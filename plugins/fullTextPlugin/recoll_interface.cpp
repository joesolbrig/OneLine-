#include <errno.h>
#include <time.h>
using namespace std;

#include "convert.h"
#include "address.h"
#include "internfile.h"
#include "item.h"
#include "recoll_interface.h"
#include "mimeparse.h"

//Of all the stupid things to have to include...
//but at least the test file requires this
//#include "address.cc"

#include <QThread>
#include "plugin_interface.h"



//Comment copied from J.F.Dockes:
//  When using extended attributes, we have to use the ctime.
//  This is quite an expensive price to pay...
#ifdef RCL_USE_XATTR
#define RCL_STTIME st_ctime
#else
#define RCL_STTIME st_mtime
#endif // RCL_USE_XATTR


using namespace Binc;


QString Recoll_Interface::bumpIPath(QString iPath){
    QString res = iPath;
    do {
        if(!res.contains(":")){
            int i = res.toInt();
            if(i<=0){ return "";}
            res = QString::number(i+1);
            continue;
        }
        QStringList paths = res.split(":");
        int i = paths[paths.length()-1].toInt();
        QString newEnd = QString::number(i+1);
        paths[paths.length()-1] = newEnd;
        res = paths.join(":");
        continue;
    } while(m_usedSubpath.contains(res));
    return res;
}

int Recoll_Interface::getItemFromQuerries(Rcl::SearchData * sdata,
                     Rcl::Query *query, QList<RecollQueryItem>& results){

    RefCntr<Rcl::SearchData> rsData(sdata);
    string qr("Query results");

    DocSequenceDb *src = new DocSequenceDb(
            RefCntr<Rcl::Query>(query), qr, rsData);

    int i=0;
    for (; i < result_limit; i++){
        Rcl::Doc doc;
        if (!query->getDoc(i, doc)) {break;}

        RecollQueryItem recollItem;
        recollItem.abstractText = src->getAbstract(doc).c_str();
        recollItem.mimeStr = doc.mimetype.c_str();
        recollItem.filePath = QString(doc.url.c_str()) + ARG_SEPERATOR + doc.ipath.c_str();
        if(recollItem.filePath.contains(FILE_PREFIX)){
            recollItem.filePath.remove(0,FILE_PREFIX.length());
        }
        if(!doc.text.empty()){
            qDebug() << "doc getItemFromQuerries has text: ";
            recollItem.longText = doc.text.c_str();
            recollItem.longText.detach();
        }

        list<string> terms;
//                vector<vector<string> > dummyGroups;
//                vector<int> dummyGslks;
        query->getMatchTerms(doc, terms);
        //src->getTerms(terms, dummyGroups, dummyGslks);
        for(list<string>::iterator i=terms.begin(); i!=terms.end(); i++){
            recollItem.resultTerms.append((*i).c_str());
        }
        recollItem.matchDescription = src->getDescription().c_str();
        results.push_back(recollItem);
    }
    return i;
}


int Recoll_Interface::getMatches(QString queryStr,  QString searchDir, QList<RecollQueryItem>& results, int l,
               QString& error,
               bool exclusive){
    result_limit = l;
    string u8 = (const char* )queryStr.toUtf8();
    trimstring(u8);

    //openDB(false);

    Rcl::SClType qt;
    if(exclusive){
        qt = SCLT_OR;
    } else {
        qt = SCLT_OR;
    }

    Rcl::SearchData * sdata = new Rcl::SearchData(Rcl::SCLT_OR);
    Rcl::SearchDataClause* clp = new Rcl::SearchDataClauseSimple(Rcl::SCLT_OR,u8);
    sdata->addClause(clp);
    string stemlang("english");
    sdata->setStemlang(stemlang);
    if(!searchDir.isEmpty())
        { sdata->setTopdir(searchDir.toStdString()); }
    Rcl::Query *query = new Rcl::Query(m_rcldb);
    RefCntr<Rcl::SearchData> rsData(sdata);
    if (!query ) {
        error = "Query Empty Somehow";
        qDebug() << "Recoll error:" << error;
        return 0;
    }

    //This index is querried here
    if ( !query->setQuery(rsData)) {
        error = rsData->getReason().c_str();
        qDebug() << "Recoll error:" << error;
        return 0;
    }
    query->setCollapseDuplicates(true);
    bool success = getItemFromQuerries(sdata, query, results);

    closeDB();
    return success;
}



bool Recoll_Interface::addAPossiblyCompoundFile(CatItem& parent, QList<CatItem>* res, int maxPasses){
    std::string ipath;
    m_continueOnIPath = true;

    int compoundCycles = 0;
    do {
        compoundCycles += addPossiblyCompoundFile(parent, res, ipath);
        if(!m_continueOnIPath){
            break;
        }
        if(!ipath.empty()){
            m_baseIpath = bumpIPath(m_baseIpath);
            parent.setLastUpdatePath(m_baseIpath);
        }
        m_usedSubpath.clear();
        //THREAD_SLEEP;
    } while(!ipath.empty() && !m_baseIpath.isEmpty() && compoundCycles < maxPasses);

    //Add if the
    return ipath.empty();
}

bool Recoll_Interface::addNonCompoundFile(CatItem& parent, QFileInfo fi, QList<CatItem>* res){
    std::string ipath;
    return addPossiblyCompoundFile(parent, res, ipath, fi);
}


//adds file to recol database, finds the mime type,
//adds internal children to CatItem and returns, woohooo!
int Recoll_Interface::addPossiblyCompoundFile(CatItem& parentIt, QList<CatItem>* res, std::string& ipath,
                                              QFileInfo fi){
    DebugLog::getdbl()->setloglevel(DEBDEB3);
    QFileInfo f;
    bool allowCompound = true;
    if(fi.filePath().isEmpty()){
        QFileInfo pathFile(parentIt.getPath());
        f = pathFile;
        allowCompound = false;
    }

    //Foo - TODO - crashing on zips for me
    if(f.suffix()=="zip"){
        ipath.clear();
        return 1;
    }

    if(!f.exists()){
        m_continueOnIPath=false;
        return 0;
    }

    //const std::string fn = parent.getPath().toStdString();
    const std::string d = f.absoluteDir().absolutePath().toStdString();
    m_rclconfig->setKeyDir(d);

    std::string fn = parentIt.getPath().toStdString();
    m_baseIpath = parentIt.getLastUpdatePath();
    std::string savedIpath = m_baseIpath.toStdString();

    const char* filePathPtr = fn.c_str();
    if (lstat(filePathPtr, &m_stp) != 0) {
        LOGERR(("Recoll_Interface::addPossiblyCompoundFile: lstat(%s): %s", fn.c_str(),
            strerror(errno)));
        m_continueOnIPath=false;
        return 0;
    }
    if(checkFileInDatabase(fn) && ipath.empty()) {
        ipath.clear();
        return 1;
    }

    LOGDEB0(("addPossiblyCompoundFile: processing: [%s] %s\n",
             displayableBytes(m_stp.st_size).c_str(), fn.c_str()));

    m_parent_udi.clear();
    make_udi(fn, savedIpath, m_parent_udi);

    m_base_indexed = false;
    m_indexingJustParent = false;
    if(!savedIpath.empty()){
        //fn = m_parent_udi;
        ipath = savedIpath;
    }
    //qDebug() << "addPossiblyCompoundFile at updatePath: " << fn.c_str();
    FileInterner interner(fn, &m_stp, m_rclconfig, m_tmpdir, FileInterner::FIF_none);
    // File name transcoded to utf8 for indexation.
    string charset = m_rclconfig->getDefCharset(true);
    // If this fails, the file name won't be indexed, no big deal
    // Note that we used to do the full path here, but I ended up believing
    // that it made more sense to use only the file name

    string utf8fn;
    int ercnt;
    if (!transcode(path_getsimple(fn), utf8fn, charset, "UTF-8", &ercnt)) {
        LOGERR(("addPossiblyCompoundFile: fn transcode failure from [%s] to UTF-8: %s\n",
                charset.c_str(), path_getsimple(fn).c_str()));
    } else if (ercnt) {
        LOGDEB(("addPossiblyCompoundFile: fn transcode %d errors from [%s] to UTF-8: %s\n",
                ercnt, charset.c_str(), path_getsimple(fn).c_str()));
    }
    LOGDEB2(("addPossiblyCompoundFile: fn transcoded from [%s] to [%s] (%s->%s)\n",
             path_getsimple(fn).c_str(), utf8fn.c_str(), charset.c_str(),
             "UTF-8"));

    int maxPasses = parentIt.getMaxUpdatePasses();
    bool cont = true;
    int cycles=0;
    while((cycles < maxPasses) && cont && (allowCompound || cycles==0)) {
        CatItem addedItem;
        cont = processInternalFile(interner,parentIt, addedItem, utf8fn, ipath);
        if(!addedItem.isEmpty()){ res->append(addedItem); }
        cycles++;
        if(!cont){break;}
        ipath.clear();
        //THREAD_SLEEP;
    }
    Q_ASSERT(cycles>0);
    return cycles;

}

bool Recoll_Interface::getDocForPreview(CatItem& it){
    string basePath;
    string ipath;
    if(it.getPath().contains(ARG_SEPERATOR)){
        QStringList parts = it.getPath().split(ARG_SEPERATOR);
        basePath = parts[0].simplified().toStdString();
        ipath = parts[1].toStdString();
    } else {
        basePath = it.getPath().toStdString();
    }
    QFileInfo f(basePath.c_str());
    //Foo - TODO - crashing on zips for me
    if(f.suffix()=="zip"){ 
        qDebug() << "skipping zip";
        return false; 
    }
    if(!f.exists()){
        qDebug() << "base path file doesn't exist: " << it.getPath();
        return false;
    }

    Rcl::Doc outDoc;

    const char* filePathPtr = basePath.c_str();
    if (lstat(filePathPtr, &m_stp) != 0) {
        qDebug() << "lstat error" << strerror(errno);
        return false;
    }

    m_parent_udi.clear();
    Q_ASSERT(m_tmpdir.length()>0);
    
    FileInterner interner(basePath, &m_stp, m_rclconfig, m_tmpdir, FileInterner::FIF_forPreview);

    try {
        FileInterner::Status ret = interner.internfile(outDoc, ipath);
        if (ret == FileInterner::FIDone || ret == FileInterner::FIAgain) {
            // FIAgain is actually not nice here. It means that the record
            // for the *file* of a multidoc was selected.
            CatItem baseIt(basePath.c_str());
            it = makeCatItem(outDoc,baseIt);
            if (!interner.get_html().empty()) {
                QString preview = interner.get_html().c_str();
                preview.detach();
                it.setPreviewHtml(preview);
            } else if(it.getLongText().isEmpty()){
                qDebug() << "getDocForPreview - don't have preview str for: " << it.getPath();
                return false;
            }
        } else {
            string missing;
            interner.getMissingExternal(missing);
            qDebug() << "getDocForPreview - can't get preview. Err: "
                    << missing.c_str() << "ret:" << ret << " path: " << it.getPath();
            return false;
        }
    } catch (...) {
        qDebug() << "getDocForPreview - some processing err: ";
        return false;
    }
    return true;
}

bool Recoll_Interface::processInternalFile(FileInterner& interner, CatItem& parentIt, CatItem& returnedItemIt,
        string utf8fn, string& ipath){

    //receiver objects!
    Rcl::Doc doc;
    FileInterner::Status fis = FileInterner::FIError;
    //ipath.clear();

    if(!m_indexingJustParent){
        //Let see if we can stuff the info in doc
        if(!ipath.empty()){
            try {
                fis = interner.internfile(doc, ipath);
                ipath.clear();
            } catch(...){
                qDebug() << "exception for error externing interning" << m_parent_udi.c_str();
            }
        }

        try { fis = interner.internfile(doc, ipath); }
        catch (CancelExcept) {
            LOGERR(("Recoll_Interface::addPossiblyCompoundFile: interrupted\n"));
            Q_ASSERT(false);
            return false;
        } catch(...){
            qDebug() << "exception for error interning" << m_parent_udi.c_str();
        }
        if(fis == FileInterner::FIError){
            qDebug() << "error interning" << m_parent_udi.c_str();
            qDebug() << interner.getReason().c_str();
            return false;
        }
    }

    //Add bookmark for our item
    QString newInternalPath(ipath.c_str());
    if(m_indexingJustParent || fis == FileInterner::FIDone ){
        if( (ipath.empty() )){
            m_continueOnIPath= false;
        }
        //qDebug() << "done interning " << m_parent_udi.c_str();
        //qDebug() << interner.getReason().c_str();
        return false;
    }

    qDebug() << "newInternalPath: " << ipath.c_str();
    parentIt.setLastUpdatePath(newInternalPath);

    //Setup Recol Document more...
    // Set file name, mod time and url if not done by filter
    if (doc.fmtime.empty()) {
        char ascdate[20];
        sprintf(ascdate, "%ld", long(m_stp.st_mtime));
        doc.fmtime = ascdate;
    }
    char cbuf[100];
    sprintf(cbuf, "%ld", (long)m_stp.st_size);
    doc.fbytes = cbuf;
    // Document signature for up to date checks: concatenate
    // m/ctime and size. Looking for changes only, no need to
    // parseback so no need for reversible formatting. Also set,
    // but never used, for subdocs.
    sprintf(cbuf, "%ld%ld", (long)m_stp.st_size, (long)m_stp.RCL_STTIME);
    doc.sig = cbuf;
    // Index at least the file name even if there was an error.
    // We'll change the signature to ensure that the indexing will
    // be retried every time.
    // Internal access path for multi-document files
    // If there was an error, ensure indexing will be
    // retried. This is for the once missing, later installed
    // filter case. It can make indexing much slower (if there are
    // myriads of such files, the ext script is executed for them
    // and fails every time)
    if (fis == FileInterner::FIError) {
        const string plus("+");
        doc.sig += plus;
    }
    if (doc.url.empty())
        {doc.url = FILE_PREFIX.toStdString() + parentIt.getPath().toStdString();}
    if (doc.utf8fn.empty()){
        doc.utf8fn = utf8fn;
    }
    // Add document to database. If there is an ipath, add it as a children
    // of the file document.
    if (ipath.empty()) {
        if(addRecolMetadataToParent(doc, parentIt)){
            returnedItemIt = parentIt;
        }
        m_base_indexed = true;
        m_indexingJustParent = false;
        m_rcldb->addOrUpdate(m_parent_udi, "", doc);
        qDebug() << "no internal file to add in loop, this is main file with regular parent";
    } else{
        doc.ipath = ipath;
        string udi;
        make_udi(parentIt.getPath().toStdString(), ipath, udi);
        //CatItem childIt(COMPOUND_FILE_PREFIX + parentIt.getPath() + META_ITEM_SEP + QString(doc.ipath.c_str()));
        CatItem childIt = makeCatItem(doc, parentIt);
        CatItem parentSubstitute = parentIt;
        childIt.addParent(parentSubstitute);
        m_rcldb->addOrUpdate(udi, m_parent_udi, doc);
        //qDebug() << "adding internal child: " << childIt.getPath();
        returnedItemIt = childIt;
    }

    if(fis != FileInterner::FIAgain){
        if(!m_base_indexed){
            m_base_indexed = true;
            m_indexingJustParent = true;
            return true;
        }
        parentIt.setSourceUpdateTime(
                appGlobalTime());
        return false;
    }
    if((!newInternalPath.isEmpty() && newInternalPath==m_baseIpath) ||
        m_usedSubpath.contains(newInternalPath) || newInternalPath.length() >12){
        return false;
    } else {
        m_usedSubpath.insert(newInternalPath);
        return true;
    }
}

bool Recoll_Interface::addRecolMetadataToParent(Rcl::Doc doc,CatItem &it){

    bool changed=false;
    if(!doc.mimetype.empty() ){
        //The information added here is incomplete
        //the rest of the mime-info is added when the item is loaded...
        it.setActionType(doc.mimetype.c_str());
        changed=true;
    }

    QString body;
    if(!doc.text.empty()){
        body = doc.text.c_str();
        if(!body.isEmpty()) {
            it.setTemporaryLongText(body);
            changed=true;
        }
    }
    return changed;
}


bool Recoll_Interface::checkFileInDatabase(const std::string &fn){
    ////////////////////
    // Check db up to date ? Doing this before file type
    // identification means that, if usesystemfilecommand is switched
    // from on to off it may happen that some files which are now
    // without mime type will not be purged from the db, resulting
    // in possible 'cannot intern file' messages at query time...

    // Document signature. This is based on m/ctime and size and used
    // for the uptodate check (the value computed here is checked
    // against the stored one). Changing the computation forces a full
    // reindex of course.
    char cbuf[100];
    sprintf(cbuf, "%ld%ld", (long)m_stp.st_size, (long)m_stp.RCL_STTIME);
    string sig = cbuf;
    string udi;
    make_udi(fn, "", udi);
    if (!m_rcldb->needUpdate(udi, sig)) {
        LOGDEB(("checkFileInDatabase: up to date: %s\n", fn.c_str()));
        return true;
    }
    return false;
}


CatItem Recoll_Interface::makeCatItem(Rcl::Doc doc, CatItem& parentIt){

    //---------------
    // This function should actually depend on the mime type but email
    // is *ABOUT* all I'm doing now...
    //---------------

    QString body;
    if(!doc.text.empty()){
        body = doc.text.c_str();
        body.detach();
    }
    string mgsid("msgid");
    QString localPath = parentIt.getPath()
            + ARG_SEPERATOR + QString(doc.ipath.c_str());
    if(!(doc.meta.find(string(mgsid)) !=doc.meta.end())){
        CatItem res(localPath);
        if(!doc.mimetype.empty()){
            res.setActionType(doc.mimetype.c_str());
        }
        res.setLongText(body);
        if(!doc.fmtime.empty()){
            time_t numeric_time = rfc2822DateToUxTime(doc.fmtime);
            if(((int)numeric_time) > 0){
                //marking time explicitly matters...
                res.setCreationTime(numeric_time,true);
                CatItem sourceParent(addPrefix(ACTION_TYPE_PREFIX,EMAIL_ADDRESS_NAME));
                res.setExternalWeightFromTime(numeric_time, sourceParent);
            }
        }
        res.setLabel(FULL_TEXT_PLUGIN_NAME);
        return  res;
    }
    QString name = doc.meta[SUBJECT_STR].c_str();
    if(name.isEmpty()) {
        name = "NO SUBJECT";
    }

    const string msgId = doc.meta[mgsid];
    CatItem res(EMAIL_ITEM_PREFIX + msgId.c_str(),name);
    if(QString(doc.meta[SUBJECT_STR].c_str()).isEmpty()) {
        res.setLabel(NAME_IS_TITLE_KEY_STR);
    }
    res.setLongText(body);
    res.setLocalStoragePath(localPath);
    res.setItemType(CatItem::MESSAGE);
    if(!doc.mimetype.empty()){
        res.setActionType(doc.mimetype.c_str());
    }

    if(!doc.fmtime.empty()){
        time_t numeric_time = rfc2822DateToUxTime(doc.fmtime);
        if(((int)numeric_time) > 0){
            res.setCreationTime(numeric_time,true);
            res.setExternalWeightFromTime(numeric_time,
                CatItem(addPrefix(TYPE_PREFIX,QString::number((int)CatItem::MESSAGE))));
        } else {
            res.setExternalWeight(MEDIUM_EXTERNAL_WEIGHT,
                CatItem(addPrefix(TYPE_PREFIX,QString::number((int)CatItem::MESSAGE))));
        }
    }

    res.setActionType(RFC_EMAIL_MESSAGE_NAME);
    if(doc.meta.find(FROM_STR.toStdString()) !=doc.meta.end()){
        const string line = doc.meta[FROM_STR.toStdString() ];
        res.setCustomPluginInfo(FROM_STR, line.c_str());

        std::vector<std::string> d;
        Binc::splitAddr(Binc::unfold(line),d);
        for (vector<string>::const_iterator i = d.begin(); i != d.end(); ++i){
            Address addr(*i);
            QString fullEmailAddress = QString(addr.local.c_str())
                              + QString("@")
                              + QString(addr.host.c_str());
            if(!fullEmailAddress.isEmpty()){
                QString senderName;
                if(!addr.name.empty()){
                    senderName = addr.name.c_str();
                } else {
                    senderName = fullEmailAddress;
                }

                QString namePath = senderName;
                CatItem contactP(PERSON_PREFIX + namePath, senderName);
                contactP.setItemType(CatItem::PERSON);

                CatItem emailP(SYNONYM_PREFIX + namePath+ "/" + fullEmailAddress,senderName);
                emailP.setItemType(CatItem::PERSON);
                emailP.setIsIntermediateSource();
                emailP.addParent(contactP,FULL_TEXT_PLUGIN_NAME,BaseChildRelation::SYNONYM);

                res.setAuthor(senderName);
                res.setEmailAddr(fullEmailAddress);
                res.addParent(contactP);
                res.setTemporaryDescription("from: " + fullEmailAddress);
            }
        }
    }
    if(doc.meta.find(TO_STR.toStdString()) !=doc.meta.end()){
        const string line = doc.meta[TO_STR.toStdString() ];
        res.setCustomPluginInfo(TO_STR, line.c_str());

        std::vector<std::string> d;
        Binc::splitAddr(Binc::unfold(line),d);
        for (vector<string>::const_iterator i = d.begin(); i != d.end(); ++i){
            Address addr(*i);
            QString fullEmailAddress = QString(addr.local.c_str())
                              + QString("@")
                              + QString(addr.host.c_str());
            CatItem emailAccount(fullEmailAddress);
            emailAccount.setItemType(CatItem::ORGANIZING_TYPE);
            emailAccount.setTagLevel(CatItem::EXTERNAL_SOURCE);
            if(!addr.name.empty()){
                emailAccount.setName(EMAIL_ACCOUNT_NAME_DESCRIPTION + QString(addr.name.c_str()));
            } else {
                emailAccount.setName(fullEmailAddress);
            }
            emailAccount.setSourceWeight(MAX_EXTERNAL_WEIGHT, CatItem::createTypeParent(CatItem::MESSAGE));
            res.addParent(emailAccount,FILE_CATALOG_PLUGIN_STR,BaseChildRelation::MESSAGE_SOURCE_PARENT);
        }
    }
    qDebug() << "recoll interface - creating item: " << res.getPath();
    //qDebug() << "long text:" << res.getLongText();
    Q_ASSERT(res.getLongText() == body);
    return res;
}
