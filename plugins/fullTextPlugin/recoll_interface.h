#ifndef RECOLL_INTERFACE_H
#define RECOLL_INTERFACE_H
#include <QString>
#include <list>
#include <string>
#include <QDebug>
#include <sys/stat.h>

#include "debuglog.h"
#include "rclinit.h"
#include "indexer.h"
#include "smallut.h"
#include "pathut.h"
#include "rclmon.h"
#include "x11mon.h"
#include "cancelcheck.h"
#include "rcldb.h"
#include "beaglequeue.h"
#include "recollindex.h"
#include "fsindexer.h"
#include "fileudi.h"

#include "rcldb.h"
#include "rclquery.h"
#include "rclconfig.h"
#include "pathut.h"
#include "rclinit.h"
#include "debuglog.h"
#include "wasastringtoquery.h"
#include "wasatorcl.h"
#include "wipedir.h"
#include "transcode.h"

#include "plaintorich.h"
#include "docseqdb.h"
#include "internfile/internfile.h"


#include "item.h"

//#define RFC822_TIME_FORMAT "%a, %d %b %Y %H:%M:%S %z"

struct RecollQueryItem{
    QString filePath;
    QString abstractText;
    QString matchDescription;
    QString mimeStr;
    QStringList resultTerms;
    QString longText;
};

using namespace Rcl;
extern RclConfig *rclconfig;

class PluginUpdater : public DbIxStatusUpdater {
    static bool stopindexing;
 public:
    virtual bool update() {
        if (stopindexing) {
            return false;
        }
        return true;
    }
};

static PluginUpdater updater;


static void recollCleanup(){ }
static void sigcleanup(int)
{
    fprintf(stderr, "sigcleanup called\n");
    stopindexing=1;
}


class Recoll_Interface{
    QSet<QString> m_visitedPath;
    public:
        Recoll_Interface(QString dbPathName=""){ //:m_fileProcessor(0)
            DebugLog::getdbl()->setloglevel(DEBDEB3);
            DebugLog::setfilename("/home/hansj/recol_test_err.txt");
            if(!dbPathName.isEmpty()){
                QString error;
                createDB(dbPathName, error);
            }
            m_db_is_open = false;
        }
        ~Recoll_Interface(){
            deleteZ(m_rclconfig);
            if(m_confindexer){
                deleteZ(m_confindexer);
            }
        }
        void setTempDir(QString tempDir){
            m_tmpdir = tempDir.toStdString();
        }

//        void cleanup(){
//            if(m_rclconfig){
//                delete m_rclconfig;
//                m_rclconfig=0;
//            }
//        }




        // Code for creating stemming databases. We also remove those which are not
        // configured.
//        bool createStemmingDatabases()
//        {
//            string slangs;
//            if (m_rclconfig->getConfParam("indexstemminglanguages", slangs)) {
//                list<string> langs;
//                stringToStrings(slangs, langs);
//
//                // Get the list of existing stem dbs from the database (some may have
//                // been manually created, we just keep those from the config
//                list<string> dblangs = this->m_rcldb->getStemLangs();
//                list<string>::const_iterator it;
//                for (it = dblangs.begin(); it != dblangs.end(); it++) {
//                    if (find(langs.begin(), langs.end(), *it) == langs.end())
//                        m_rcldb->deleteStemDb(*it);
//                }
//                for (it = langs.begin(); it != langs.end(); it++) {
//                    m_rcldb->createStemDb(*it);
//                }
//            }
//            return true;
//        }

        bool createDB(QString& dbPathName, QString& error){
            string reason;
            string a_config = dbPathName.toStdString();
            if(!m_rclconfig){
                m_rclconfig = recollinit(recollCleanup, sigcleanup, reason, &a_config);
                rclconfig = m_rclconfig;
            }
            if (!m_rclconfig || !m_rclconfig->ok()) {
                error = reason.c_str();
                qDebug() << "recol config error:" << error;
                return false;
            }
            if(!m_rcldb){
                m_rcldb = new Rcl::Db(m_rclconfig);
            }

            return true;
        }

        bool removeFile(QString filePath){
            const string fn = filePath.toStdString();
            string udi;
            make_udi(fn, "", udi);
            return m_rcldb->purgeFile(fn);
        }


        bool openDB(bool write){
//            if(m_db_is_open){
//                qDebug() << "Somehow recol's already open, leave";
//                return false;
//            }
            Q_ASSERT(!m_db_is_open);
            m_db_is_open = true;
            Rcl::Db::OpenMode mode;
            if (write){
                mode = Rcl::Db::DbUpd;
//                if(trunc){
//                    mode = Rcl::Db::DbTrunc;
//                } else {
//                    mode = Rcl::Db::DbUpd;
//                    mode = Rcl::Db::DbUpd;
//                }
            } else {
                mode = Rcl::Db::DbRO;
            }
            if (!m_rcldb->isopen() && !m_rcldb->open(mode)) {
                QString error = this->m_rcldb->getReason().c_str();
                qDebug() << "Recoll error:" << error;
                m_db_is_open = false;
                return false;
            }
            return true;
        }

        void closeDB(){
            if(!m_db_is_open){
                qDebug() << "Somehow recol's not open, leave";
                return;
            }
            m_db_is_open = false;
            //It *seems* that close kills the whole thing and shoudl

            //m_rcldb->close();
        }

        void purgeDB(){
            m_rcldb->purge();
        }

        bool addAPossiblyCompoundFile(CatItem& parent, QList<CatItem>* res, int maxPasses);
        bool addNonCompoundFile(CatItem& parent, QFileInfo fi, QList<CatItem>* res);
        int addPossiblyCompoundFile(CatItem& parent, QList<CatItem>* res,
            std::string& ipath, QFileInfo fi=QFileInfo());
        QString bumpIPath(QString iPath){
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

//        std::string bumpIPath(std::string ip){
//            QString iPath(ip.c_str());
//            if(m_visitedPath.contains(iPath)){
//                if(!iPath.contains(":")){
//                    int i = iPath.toInt();
//                    Q_ASSERT(i>0);
//                    return QString::number(i+1);
//                }
//                QStringList paths = iPath.split(":");
//                int i = paths[paths.length()-1].toInt();
//                QString newEnd = QString::number(i+1);
//                paths[paths.length()-1] = newEnd;
//                QString res = paths.join(":");
//                return res.toStdString();
//            }
//            m_visitedPath.insert(iPath);
//            return "";
//        }
        int getMatches(QString queryStr,  QString searchDir, QList<RecollQueryItem>& results, int l,
                       QString& error,
                       bool exclusive=false){
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

        bool getDocForPreview(CatItem& it);
    protected:

        int getItemFromQuerries(Rcl::SearchData * sdata,
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


        bool processInternalFile(FileInterner& interner, CatItem& parentIt, CatItem& childIt, string utf8fn, string& beginIPath);
        //void finishParent(FileInterner& interner,CatItem& parent);
        static CatItem makeCatItem(Rcl::Doc doc, CatItem& parentIt);
        bool addRecolMetadata(Rcl::Doc doc, CatItem& item);
        bool checkFileInDatabase(const std::string &fn);
    private:
        RclConfig* m_rclconfig;

        Rcl::Db* m_rcldb;
        string m_tmpdir;
        string m_parent_udi;
        bool m_db_is_open;
        bool m_base_indexed;
        bool m_indexingJustParent;

        QString m_baseIpath;
        bool m_continueOnIPath;
        QSet<QString> m_usedSubpath;

        int result_limit;

        QString dbPathName;
        struct stat m_stp;

        //Old for testing...
        ConfIndexer* m_confindexer;


};



#endif // RECOLL_INTERFACE_H
