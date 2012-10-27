
#include <QRegExp>
#include "item.h"
#include "list_item.h"
#include "input_list.h"
//#include "appearance_window.h"

QString summarizeMessage(CatItem it){
    QString res;

    if(it.hasLabel(NAME_IS_TITLE_KEY_STR)){
        res+= it.getName() + " ";
    }

    if(it.hasLabel(AUTHOR_KEY_STR)){
        res+= QString("<br>") + FROM_PERSON__PHRASE + it.getAuthor();
    }

    if(!it.getDescription().isEmpty()){
        res+=it.getDescription();
    } else if(!it.getLongText().isEmpty()){
        res+=it.getLongText().left(UI_MAX_MESSAGE_DESCRIPTION_LEN);
    }

    if(it.hasLabel(TIME_EXPLICITELY_SET_KEY_STR)){
        res+="(";
        time_t tm = (MAX(it.getCreationTime(), it.getModificationTime()));
        QDateTime t;
        t.fromTime_t(tm);
        res+= contextualTimeString(t);
        res+=")";
    }

    return res;
}



void drawStyleRect(QWidget* caller, QRect containingRect){
    QPainter painter(caller);
    painter.drawRect(containingRect);
}


void drawBottomHtmlLine(QPainter *painter, const QFont font, QRect rect, QString text);

int drawFieldLabel(QPainter *painter, QFont font,
                       QString htmlFieldLabel, QRect rect,
                       bool bottomAligned){

    int fieldNameFontSize = fontSize4Width(
            crudeHtmlFragmentToPlainText(htmlFieldLabel), rect.size(), font);
    font.setPointSize(fieldNameFontSize);
    QFontMetrics fm(font);
    rect.setHeight(fm.height()+ fm.leading()*2+10);
    if(bottomAligned){
        drawBottomHtmlLine(painter, font, rect, htmlFieldLabel);
    } else {
        drawHtmlLine(painter, font, rect, htmlFieldLabel);
    }
    return rect.height();
}

int drawFieldAndValue(QPainter *painter, QFont font,
                       QString htmlField, QString htmlFieldValue, QRect rect){

    QRect rightRect = rect;
    rightRect.moveRight(rect.right() - rect.width()*(1 - FIELD_NAME_RIGHTSPACE_RATIO));

    int fieldNameFontSize = fontSize4Width(
            crudeHtmlFragmentToPlainText(htmlField), rightRect.size(), font);
    QString txt = htmlFontSizeFormat(htmlField, fieldNameFontSize) + " " + htmlFieldValue;
    bool breakLine = (fieldNameFontSize < FIELD_NAME_FONT_STANDARDSIZE);
    if(painter){
        drawHtmlLine(painter, font, rect, txt, breakLine);
    }
    return fieldNameFontSize + UI_FIELD_FONT_MARGIN;

}

int drawJustField(QPainter *painter, QFont font,
                      QString htmlField, QRect rect){

    //rightRect.moveRight(rect.right() - rect.width()*(1 - FIELD_NAME_RIGHTSPACE_RATIO));
    htmlField = tagAs(htmlField, "b");
    htmlField = tagAs(htmlField, "u");

    int fieldNameFontSize = fontSize4Width(
            crudeHtmlFragmentToPlainText(htmlField), rect.size(), font);
    QString txt = htmlFontSizeFormat(htmlField, fieldNameFontSize);
    if(painter){
        drawHtmlLine(painter, font, rect, txt);
    }
    return rect.height() ;

}

int fontSize4Width(QString //t
                   , QSize //size
                   , QFont //inFont
                   ){
    return UI_INPUT_LABEL_MIN_SIZE;

    //qDebug() << "sizing: " << t;

//    int mid;
//    int upperFontSize = UI_INPUT_LABEL_MAX_SIZE;
//    int lowerFontSize = UI_INPUT_LABEL_MIN_SIZE;
//    while(abs(upperFontSize - lowerFontSize) >1){
//        QFont font = inFont;
//        mid = (upperFontSize + lowerFontSize)/2;
//        font.setPointSize(mid);
//        QFontMetrics fm(font);
//        QSize textSize = fm.size(Qt::TextSingleLine,t);
//        if(textSize.width() > size.width()  &&
//           textSize.height() > size.height()){
//            lowerFontSize = mid;
//        } else { upperFontSize = mid; }
//
//    }
//    //qDebug() << "pixel size: " << lowerFontSize;
//    return lowerFontSize;
}

QString crudeHtmlFragmentToPlainText(QString html){
    QString res;
    bool inDoubleQuote=false;
    bool inSingleQuote=false;
    bool inTag=false;
    for(int i=0; i< html.length();i++){
        if(!inTag){
            if(html[i]!='<'){
                res.append(html[i]);

            } else {
                inTag =true;
            }
        } else if(!inDoubleQuote && !inSingleQuote){
            if(html[i]=='\"'){
                inDoubleQuote = true;
            }
            if(html[i]=='\''){
                inSingleQuote = true;
            }
            if(html[i]=='>'){
                inTag =false;
            }
        } else if(inDoubleQuote ){
            if(html[i]=='\"'){
                inDoubleQuote = false;
            }
        } else if(inSingleQuote){
            if(html[i]=='\''){
                inSingleQuote = false;
            }
        } //else just increment...
    }
    return res;
}


void setUpDisplayDoc(QTextDocument& displayDoc, const QFont font){
    QTextOption to;
    //to.setWrapMode(QTextOption::NoWrap);
    to.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    displayDoc.setDefaultTextOption(to);

    displayDoc.setDefaultFont(font);
    displayDoc.setDocumentMargin(0); //default is 4!
    displayDoc.setIndentWidth(0);
    displayDoc.setDefaultStyleSheet( UI_LIST_LINE_DEFAULT_STYLE);

}

void drawBottomHtmlLine(QPainter *painter, const QFont font, QRect rect, QString text){
    QRect r = rect;
    QFontInfo fi(font);
    int height = fi.pixelSize();
    r.moveTop(r.bottom() - height);
    drawHtmlLine(painter, font, r, text) ;

}

void drawHtmlRightAligned(QPainter *painter, const QFont font, QRect rect, QString text){
    drawHtmlLine(painter, font, rect, text, false, true);

}

void drawHtmlLine(QPainter *painter, const QFont font, QRect rect, QString text, bool multiline,
                  bool leftAligned) {
    if(!painter){return;}

    painter->save();

    QTextDocument displayDoc;
    setUpDisplayDoc(displayDoc, font);
    if(!leftAligned){
        text = QString("<div align=\"right\">") + text + "</div>";

    }

    displayDoc.setHtml(text);
    //multiline == false - Normally
    if(multiline){
        displayDoc.setTextWidth(rect.width());
    } else {displayDoc.setTextWidth(-1);}


    painter->translate(rect.topLeft());

    if(multiline){ displayDoc.adjustSize(); }

    rect.moveTopLeft(QPoint(0,0));
    displayDoc.drawContents(painter, rect);

    painter->restore();

}


//This is for one line of html-formatted text
qreal pixelOffsetForHtmlChar( QFont font, QString text, int index) {
    if(index==-1){
        index = text.length()-1;
    }

    QTextLayout tl(text,font);

    tl.beginLayout();
    QTextLine line = tl.createLine();
    Q_ASSERT( line.isValid() );
    line.setLineWidth(1000);
    int pos = line.cursorToX(index);
    tl.endLayout();

    return pos;

}

//qreal pixelOffsetForHtmlChar( QFont font, QString text, int index) {
//    QTextDocument displayDoc;
//    setUpDisplayDoc(displayDoc, font);
//    displayDoc.setHtml(text);
//    displayDoc.setTextWidth(-1);
//
//    Q_ASSERT(displayDoc.blockCount()==1);
//
//    QAbstractTextDocumentLayout *layout = displayDoc.documentLayout();
//
//    //We're assuming a simple with only one block, the default;
//    QTextCursor tc(&displayDoc);
//    //tc.setPosition(index);
//
//    const QTextBlock textCursorBlock = displayDoc.begin();
//    const QRectF blockBoundingRect = displayDoc.documentLayout()->blockBoundingRect( textCursorBlock );
////   QTextFrameFormat rootFrameFormat = m_pTextDocument->rootFrame()->frameFormat();
//
//   const QTextLayout *textCursorBlockLayout = textCursorBlock.layout();
//
//    qreal pos=-1;
//    QTextLine textLine = textCursorBlockLayout->lineAt(0);
//    Q_ASSERT( textLine.isValid() );
//    pos = textLine.cursorToX(index);
//    return pos;
//
//}

//void GetXYWHOfLayoutIndex( int index, float *flX, float *flY, float *flW, float *flH ){
//
//   if( m_pTextCursor ) {
//       m_pTextCursor->setPosition( index );
//
//       const QTextBlock textCursorBlock = m_pTextCursor->block();
//       const QRectF blockBoundingRect = m_pTextDocument->documentLayout()->blockBoundingRect( textCursorBlock );
//       QTextFrameFormat rootFrameFormat = m_pTextDocument->rootFrame()->frameFormat();
//
//       const QTextLayout *textCursorBlockLayout = textCursorBlock.layout();
//       const int relativeCursorPositionInBlock =
//               m_pTextCursor->position() - m_pTextCursor->block().position();
//       QTextLine textLine =
//               textCursorBlockLayout->lineForTextPosition( relativeCursorPositionInBlock );
//       if ( textLine.isValid() ) {
//           int pos = m_pTextCursor->position() - m_pTextCursor->block().position();
//           *flY = textLine.lineNumber() * textLine.height() + blockBoundingRect.top();
//           *flX = textLine.cursorToX(pos) + rootFrameFormat.leftMargin() + rootFrameFormat.padding();
//           *flW = textLine.cursorToX(pos + 1) + rootFrameFormat.leftMargin() + rootFrameFormat.padding() - *flX;
//           *flH = textLine.height();
//       } else {
//           *flX = 0.0;
//           *flY = 0.0;
//           *flW = 0.0;
//           *flH = 0.0;
//       }
//   }
//   else
//   {
//   *flX = 0.0;
//   *flY = 0.0;
//   *flW = 0.0;
//   *flH = 0.0;
//   }
//   DebugLog( "GetXYWHOfLayoutIndex %.3f %.3f %.3f %.3f", *flX, *flY, *flW, *flH );
//
//}

int htmlLineCount( QFont font, QString text, int width) {

    QTextDocument displayDoc;

    setUpDisplayDoc(displayDoc,font);
    displayDoc.setHtml(text);
    displayDoc.setTextWidth(-1);

    displayDoc.setPageSize(QSizeF(width, QWIDGETSIZE_MAX));
    QFontMetrics fm(font);
    int fontHeight = fm.height();
    int lineHeight = displayDoc.documentLayout()->documentSize().height();
    return lineHeight/fontHeight;

}





//An actual visible Item
//some of its effects are manipulated by MainUserWindow,
//So its members are public for now...


//We will be creating strings by applying a
//series of functions acting on QStringList

//class TextMeasurer {
//private:
//    double m_length;
//public:
//    textMeasurer(int length){
//        m_length = length;
//    }
//
//
//    virtual void addString(QString s){
//        m_length -= s.length();
//    }
//
//    virtual bool hasMoreSpace(QString s){
//        return ((m_length - s.length()) >=0);
//    }
//
//};


ListItem::ListItem(CatItem it): CatItem(it){
    assign_from(it);
    m_useDisplayPathForNextKey = false;
    m_minSplitPointIndex = 10000;
    m_maxSplitPointIndex = -2;
    m_nextKeyIndex = -1;
    m_charsAvail = 0;
}

QList<ListItem> ListItem::convertList(QList<CatItem> cList){
    QList<ListItem> res;
    for(int i=0; i< cList.count(); i++){
        res.append(ListItem(cList[i]));
    }
    return res;

}

QString ListItem::modifiedDescription(){
    QString descript = getFormattedDescription();
    descript = descript.simplified();

    QString homeDir = QDir::homePath();
    if(descript.startsWith(homeDir)){
        descript.remove(0,(homeDir.length()));
        descript.prepend(HOME_ABREV);
    }
    return descript;
}

bool isSep(QChar c){
    if(c=='/'){ return true; }
    if(c==' '){ return true; }
    if(c=='?'){ return true; }
    if(c=='#'){ return true; }
    if(c=='~'){ return true; }
    return false;
}

bool ListItem::endSep(QString workingPath, int i){

    //if(i==0){ return false; }
//    if(i>=workingPath.count()-1)
//        { return true; }
    if(isSep(workingPath[i]) && !isSep(workingPath[i+1]))
        { return true; }
    return false;
}

QList<QString> ListItem::splitPath(QString path){
    m_componentList.clear();
    int componentStart= 0;
    for(int i=0; i<path.count(); i++){
        if(endSep(path,i)){
            m_componentList.append(path.mid(componentStart, (i+1)-componentStart));
            componentStart=i+1;
        } }
    if(componentStart < path.length())
        { m_componentList.append(path.mid(componentStart, path.length()-componentStart)); }
    return m_componentList;
}



void ListItem::createComponentList(){


    QString descript = modifiedDescription();

    m_componentList = splitPath(descript);

    m_nameStartIndex=0;
    for(int i=0; i<m_componentList.count();i++){
        int l= m_componentList[i].length();
        if(m_componentList[i][l]=='/'){
            m_nameStartIndex = i;
        }
    }

}


QString ListItem::getFormattedPath(int charsAllowed, bool keepName, bool userAlt){

    QString formatedStr;
    if(!m_componentList.isEmpty()){
        m_processedPieces.clear();
        m_displayLengths.clear();
        m_processedPieces = getPathPieces();
        m_displayLengths = decomposeByStrings(charsAllowed,m_processedPieces);
        QStringList l = truncByLength(m_processedPieces, m_displayLengths);
        l = positionOrder(l);
        m_processedPieces = positionOrder(m_processedPieces);
        reorderBounds();
        if(m_useDisplayPathForNextKey && userAlt){
            l = highlightKey(l, m_nextKeyIndex);
        }
        if((m_maxSplitPointIndex !=-2)) {
            Q_ASSERT(m_minSplitPointIndex !=m_componentList.count());
            l[1] = htmlFontFormat(l[1], EMPHESIS_COLOR);
        } else {
            Q_ASSERT(m_minSplitPointIndex ==m_componentList.count());
        }
        //l = stripNulls(l);
        formatedStr = stripCombine(l, keepName);
        formatedStr = htmlize(formatedStr);


    } else {
        formatedStr = htmlize( modifiedDescription() );
    }

    return formatedStr;
}

QString ListItem::getRawDisplayPath(int charsAvail){
    if(charsAvail==-1){
        charsAvail = m_charsAvail;
    } else {
        m_charsAvail =  charsAvail;
    }
    if(getActionType()==CatItem::NOUN_ACTION){
        QStringList l = getPathPieces();
        m_displayLengths = decomposeByStrings(charsAvail,l);
        l = truncByLength(l, m_displayLengths);

        l = positionOrder(l);
        l = stripNulls(l);
        return l.join("/");
    }
    else {
        return this->getDescription();
    }
}

QStringList ListItem::getPathPieces(){
    int lastPos = m_componentList.count()-1;
    QStringList d;
    //Create our parts in PRIORITY ORDER
    //Logic should allow all but the first to be empty

    //Split-off/Distinguishing point in path
    int startDistinguished = MAX(m_minSplitPointIndex-1,0);
    int endDistinguished = MAX(MAX(m_minSplitPointIndex+1,m_maxSplitPointIndex),0);
    d.append(sect(startDistinguished, endDistinguished)); //0

    //Last path element before name
    int lastDir = (MAX(endDistinguished, lastPos-1));
    d.append(sect(lastDir, lastDir+1)); //1

    //The name - normally but not always
    d.append(sect(lastDir+1,lastDir+2)); //2

    //String of dirs between split-point and last post
    d.append(sect(endDistinguished, lastDir)); //3 ** Remove if NULL

    //Initial string of dirs before split point
    d.append(sect(0, startDistinguished)); //5

    Q_ASSERT(d.length() ==5);

    return d;
}

QString ListItem::sect(int s, int e){
    Q_ASSERT(e>-1);
    QString t;
    e = MIN(e, m_componentList.count());

    for(int i=s; i < e; i++){
        t += (m_componentList[i]);
    }
    return t;
}




//First support classes and functions...
QString ListItem::formattedName(bool fullHtml){
    QString formatedStr = getName();
    if(!getCustomString(OVERRIDE_FORMATTED_NAME_KEY).isEmpty()){
        formatedStr =getCustomString(OVERRIDE_FORMATTED_NAME_KEY);
    } else if(!m_useDisplayPathForNextKey){
        QStringList l;
        l.append(formatedStr);
        l = highlightKey(l, m_nextKeyIndex);
        formatedStr = (l.join(" "));
        if(UI_LISTNAME_BOLD){
            formatedStr = QString("<b>") + formatedStr + "</b>";
        }
        formatedStr = QString("<p>") + formatedStr + "</p>";

    }
    if(fullHtml){
        formatedStr = htmlize(formatedStr);
    }

    return  formatedStr;
}

QStringList ListItem::highlightKey(QStringList l, int kNum){
    QStringList out;
    if(kNum < 0){
        out.append(l);
        return out;
    }
    for(int i=0; i< l.count();i++){
        int c =l[i].length() +1;// +1 for path seperator...
        if(kNum < c){
            out.append(posFormat(l[i], NEXT_KEY_FORMAT, kNum , 1));
        } else {
            out.append(l[i]);
        }
        kNum -=c;
    }
    return out;
}


void ListItem::reorderBounds(){
    QStringList o;
    QList<int> orderedLengths;

    orderedLengths.append(m_displayLengths[4]); //0
    orderedLengths.append(m_displayLengths[0]); //1
    if(MAX(m_minSplitPointIndex+1,m_maxSplitPointIndex) < m_componentList.count()-1){
        orderedLengths.append(m_displayLengths[3]); //2
    }
    orderedLengths.append(m_displayLengths[1]);//2-3
    orderedLengths.append(m_displayLengths[2]);//3-4

    m_displayLengths = orderedLengths;

}

//back to initial order remove null middle part...
QStringList ListItem::positionOrder(QStringList l){
    QStringList o;

    o.append(l[4]);//0
    o.append(l[0]);//1
    if(MAX(m_minSplitPointIndex+1,m_maxSplitPointIndex) < m_componentList.count()-1){
        o.append(l[3]);//2
    }
    o.append(l[1]);//2-3
    o.append(l[2]);//3-4

    return o;
}

QStringList ListItem::stripNulls(QStringList o){
    QStringList p;
    for(int i=0;i <o.size();i++){
        if(!o[i].isEmpty()){p.append(o[i]);}
    }
    //foo KLUDGE warning: this doesn't really belong - make this a seperate function if necessary...
    //Adds trailing and leading slashes
    QChar s(PATH_SEP[0]);

    //if it's that short, no processing will make sense...
    if(p.length()<= 1){ return p; }

    if(getPath()[0]== s && (p[0])[0]!= s)
        { p[0] = s + p[0]; }

    if(getPath()[getPath().length()-1]== s && (p[p.length()-1])[p[p.length()-1].length()-1]!= s)
        { p[p.length()-1] = p[p.length()-1]+s; }

    if(p.length() > 2 && p[0].length() >0){
        if( p[0][p[0].length()-1]==QChar(':') && ( p[1][0]!=s)){
            p[0] = p[0] + s;
        }
    }
    return p;

}

//Combined/compressed logic here...
QString ListItem::stripCombine(QStringList o, bool keepName){
    QString p;
    bool haveAddedEllide = false;
    bool lastEmpty = false;
    QChar s(PATH_SEP[0]);
    for(int i=0;i <o.size();i++){
        if(keepName && i== o.size()-1){
            p+=m_processedPieces[i];
            break;
        }
        p += o[i];
        lastEmpty = o[i].simplified().isEmpty();
        if(o[i].length() < this->m_processedPieces[i].length()){
            if(o[i].length() || !haveAddedEllide){
                p += EXTENSION_STRING;
            }
            haveAddedEllide = true;
        } else {
            haveAddedEllide = false;
        }
    }
    if(this->m_componentList[0] == HOME_ABREV){
        if(p[0]== s){
            p = p.mid(1);
        }
    }

    //if(this->getPath()[0]== s && p[0]!= s){ p=s+p; }
    p = p.simplified();
    while(p.size()> 1 && p[p.size()-1]== '/'){
        p.resize(p.size()-1);
        p = p.simplified();
    }

    if(p[p.length()-1]== s){
        p.remove(p.length()-1);
    }
    return p;
}


//Algorithm to expand/contract string sections by priority
QList<int> ListItem::decomposeByStrings(int charsAvail, QList<QString> sl){
    int total = charsAvail;
    QList<int> res;
    for(int i =0; i < sl.count() ; i++){
        int s = sl[i].count();
        if(total > s){
            res.append(s);
        } else {
            res.append(MAX(total,0));
        }
        total -= (s+1); //take into account seperator...
    }
    return res;
}

QStringList ListItem::truncByLength(QStringList sl, QList<int> n){
    Q_ASSERT(sl.count() == n.count());
    QStringList res;
    for(int i =0; i < sl.count(); i++){
        res.append(sl[i].left(n[i]));
    }
    return res;
}







