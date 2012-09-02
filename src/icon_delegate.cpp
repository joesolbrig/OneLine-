#include "icon_delegate.h"
#include <QTextDocument>
#include <QPainter>
#include <QAbstractListModel>
#include <QAbstractTextDocumentLayout>
#include "item.h"
#include "listwithdisplay.h"
#include "edit_controls.h"
#include "appearance_window.h"

QImage blurred(const QImage& image, const QRect& rect, int radius, bool alphaOnly = false)
{
    int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
    int alpha = (radius < 1)  ? 16 : (radius > 17) ? 1 : tab[radius-1];

    QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int r1 = rect.top();
    int r2 = rect.bottom();
    int c1 = rect.left();
    int c2 = rect.right();

    int bpl = result.bytesPerLine();
    int rgba[4];
    unsigned char* p;

    int i1 = 0;
    int i2 = 3;

    if (alphaOnly)
        i1 = i2 = (QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3);

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r1) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += bpl;
        for (int j = r1; j < r2; j++, p += bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c1 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += 4;
        for (int j = c1; j < c2; j++, p += 4)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r2) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= bpl;
        for (int j = r1; j < r2; j++, p -= bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c2 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= 4;
        for (int j = c1; j < c2; j++, p -= 4)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    return result;
}

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    QLabel label;
//    QImage image("image.png");
//    image =  blurred(image,image.rect(),10,false);
//    label.setPixmap(QPixmap::fromImage(image));
//    label.show();
//
//    return a.exec();
//}



void make_grayscale(QImage& in){
    if(in.format()!=QImage::Format_Indexed8){
        in = in.convertToFormat(QImage::Format_Indexed8);
    }
    Q_ASSERT(in.numColors() >0);
    Q_ASSERT(in.format() == QImage::Format_Indexed8);
    QVector<int> transform_table(in.numColors());
    for(int i=0;i<in.numColors();i++){
        QRgb c1=in.color(i);
        int avg=qGray(c1);
        transform_table[i]=avg;
    }
    in.setNumColors(256);
    for(int i=0;i<256;i++){
        in.setColor(i,qRgb(i,i,i));
    }
    int n = in.numBytes();
    for(int i=0;i< n;i++){
        in.bits()[i]=transform_table[in.bits()[i]];
    }
}

QRgb white_scale(QRgb in, QRgb tintColor){
    int redTint = qRed(tintColor);
    int greenTint = qGreen(tintColor);
    int blueTint = qBlue(tintColor);

    int redOr = qRed(in);
    int greenOr = qGreen(in);
    int blueOr = qBlue(in);

    int r = MAX(redOr, redTint);
    int g = MAX(greenOr, greenTint);
    int b = MAX(blueOr, blueTint);
    QRgb color = qRgb(r,g,b);
    return color;
}

void make_pastel(QImage* in, QColor c){
    if(in->format()!=QImage::Format_Indexed8){
        *in = in->convertToFormat(QImage::Format_Indexed8);
    }
    Q_ASSERT(in->format() == QImage::Format_Indexed8);
    //in.setNumColors(256);
    int count = in->numColors();
    for(int i=0;i<count;i++){
        QRgb c1=in->color(i);
        QRgb rgb = qRgb(c.red(),c.green(),c.blue());
        QRgb res = white_scale(c1, rgb);
        in->setColor(i,res);
    }
}

void make_grayscale2(QImage& in){
    QImage alphaChannel = in.alphaChannel();
    if(in.format()!=QImage::Format_ARGB32_Premultiplied){
        in = in.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    }
    Q_ASSERT(in.format() == QImage::Format_ARGB32_Premultiplied);
    int width = in.width();
    int height = in.height();
    for(int i=0; i<width;i++){
        for(int j=0; j<height;j++){
            QRgb color = in.pixel(i,j);
            int gv = qGray(color);
            //int a = qAlpha(color);
            color = qRgb(gv,gv,gv);
            in.setPixel(i, j, color);
        }
    }
    in.setAlphaChannel(alphaChannel);
}

void make_glow(QImage& foreground, const QColor& glowcol = UI_ITEM_ARROW_GLOW_COLOR) {
    int spreadWidth = 3;
    QImage fg = foreground;
    if (fg.format() != QImage::Format_ARGB32_Premultiplied &&
            fg.format() != QImage::Format_ARGB32)
        fg = fg.convertToFormat(QImage::Format_ARGB32);

    QRgb glow_color = glowcol.rgba();
    int ga = qAlpha(glow_color);

    QImage simpleGlow(fg.size(), QImage::Format_ARGB32_Premultiplied);
    int x, y;
    for (x = 0; x < fg.width(); ++x) {
        for (y = 0; y < fg.height(); ++y) {
            int a = qAlpha(fg.pixel(x,y)) * ga / 255;
            // glow format is argb32_premultiplied
            simpleGlow.setPixel(x,y,
                qRgba(qRed(glow_color)*a/255,
                qGreen(glow_color)*a/255,
                qBlue(glow_color)*a/255, a));
        }
    }
    QImage finalGlow(foreground.size(),QImage::Format_ARGB32);
    finalGlow.detach();
    QPainter glowPainter(&finalGlow);
    // now draw that glowed image a few times moving around the interest point to do a glow effect
    for (x = -spreadWidth; x <= spreadWidth; ++x) {
        for (y = -spreadWidth; y <= spreadWidth; ++y) {
            if (x*x+y*y > spreadWidth*spreadWidth) // don't go beyond r pixels from (0,0)
            continue;
            glowPainter.drawImage(QPoint(x,y), simpleGlow);
        }
    }
    glowPainter.drawImage(QPoint(0,0), foreground);
    foreground = finalGlow;
}

//------------
IconDelegateFormat::IconDelegateFormat(QPainter *painter, QStyleOptionViewItem option, const QModelIndex index):
    m_index(index), m_option(option) {
    m_painter = painter;
    m_baseFontHeight = -1;
    m_baseRect = option.rect;
    m_outterMargin = index.data(ROLE_TOP_MARGIN).toInt();
}

IconDelegate::IconDelegate(QObject *parent, CatItem parItem): QAbstractItemDelegate(parent) {
    m_parentItem = parItem;

    m_arrow_icon = QIcon::fromTheme(ARROW_ICON_NAME);
    m_glow_arrow_icon = makeGlow(m_arrow_icon);
    m_plus_icon = QIcon::fromTheme(PLUS_ICON_NAME);
    m_pin_icon = QIcon::fromTheme(PIN_ICON_NAME);
    m_pinned_already_icon = grayIfy(m_pin_icon,-90);
    m_pin_icon= grayIfy(m_pin_icon);
    m_depricate_icon = QIcon::fromTheme(DEPRICATE_ICON_NAME);
}


QSize IconDelegate::sizeHint(const QStyleOptionViewItem& ,
    const QModelIndex& ) const {
    int h = UI_DEFAULT_ITEM_HEIGHT;
    if(!m_parentItem.isEmpty()){
        h = m_parentItem.getItemDisplayHeight();
    }
    int w = gMainWidget->desiredListWidth();
    //qDebug() << "return size hint width: " << w;
    return QSize(w, h);
}


QIcon IconDelegate::grayIfy(QIcon ic, int rotate){
    QPixmap im1 = ic.pixmap(8);
    QPixmap im2 = ic.pixmap(16);
    if(rotate!=0){
        QTransform t;
        t.rotate(rotate);
        im1 = im1.transformed(t);
        im2 = im2.transformed(t);
    }

    QImage gray1 = im1.toImage();
    QImage gray2 = im2.toImage();
    make_grayscale2(gray1);
    make_grayscale2(gray2);

    im1 = QPixmap::fromImage(gray1);
    im2 = QPixmap::fromImage(gray2);

    QIcon res;
    res.addPixmap(im1);
    res.addPixmap(im2);
    return res;
}

QIcon IconDelegate::makeGlow(QIcon ic){
    QPixmap im1 = ic.pixmap(8);
    QPixmap im2 = ic.pixmap(16);

    QImage glow1 = im1.toImage();
    QImage glow2 = im2.toImage();
    make_glow(glow1);
    make_glow(glow2);

    im1 = QPixmap::fromImage(glow1);
    im2 = QPixmap::fromImage(glow2);

    QIcon res;
    res.addPixmap(im1);
    res.addPixmap(im2);
    return res;
}



void IconDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
						 const QModelIndex &index) const
{
    painter->save();//save so changes won't propagate
    IconDelegateFormat idf = IconDelegateFormat(painter, option, index);
    basicFormat(idf);
    drawRect(idf);
    if(!idf.m_index.data(ROLE_LONG_BODY_TEXT).toString().isEmpty()){
        drawLongBody(idf);
    } else if(idf.m_index.data(ROLE_ITEM_TYPE).toInt()== (int)CatItem::PUBLIC_POST){
        drawStandardName(idf);
        drawTime(idf);
        drawDescription(idf);
    } else if(idf.m_index.data(ROLE_ITEM_TYPE).toInt()== (int)CatItem::MESSAGE){
        //drawSummary(idf);
        drawStandardName(idf);
        drawTime(idf);
        drawDescription(idf);
    } else if(idf.m_index.data(ROLE_USE_LONG_NAME).toBool()){
        drawLongName(idf);
    } else {
        drawStandardName(idf);
        drawDescription(idf);
    }
    drawMiniArrows(idf);
    if(idf.m_index.data(ROLE_ITEM_TAG_LEVEL).toInt()>= ((int)CatItem::POSSIBLE_TAG) ){
        drawArrow(idf);
        drawNewChildrenCount(idf);
    } else if(idf.m_index.data(ROLE_HAS_CHILDREN).toBool()){
        drawMoreItemsIndicator(idf);
    }
    drawBigText(idf);
    if(idf.m_index.data(ROLE_USE_ICON).toBool()){
        paintIcon(idf);
    }
    painter->restore();//so the next item has a clean slate
}

void IconDelegate::basicFormat(IconDelegateFormat& idf) const{
    if(idf.m_index.data(ROLE_USE_ICON).toBool()){
        idf.m_leftTextBorder = UI_STANDARD_ICON_WIDTH + UI_STANDARD_ICON_BORDER;
    } else {
        idf.m_leftTextBorder = UI_STANDARD_ICON_BORDER;
    }

    int offset=idf.m_index.data(ROLE_ITEM_OFFSET).toInt();
    idf.m_baseRect.setRight(idf.m_baseRect.right() + offset);
    idf.m_descriptionFont = delegateFont();
    idf.m_nameFont = idf.m_descriptionFont;
    if(UI_LISTNAME_BOLD){
        idf.m_nameFont.setBold(true);
        idf.m_nameFont.setWeight(QFont::Black);
    }
}

void IconDelegate::drawRect(IconDelegateFormat& idf) const{

    if (idf.m_option.state & QStyle::State_Selected){
        idf.m_painter->fillRect(idf.m_baseRect, idf.m_option.palette.highlight());
        idf.m_painter->setPen(idf.m_option.palette.color(QPalette::HighlightedText));
    } else {
        QImage im(UI_LIST_BACKGROUND);
        QColor typeColor = ListItem::colorFromType((CatItem::ItemType)
                                        idf.m_index.data(ROLE_ITEM_TYPE).toInt());
        make_pastel(&im,typeColor);
        QPixmap backgroundTexture;
        backgroundTexture = QPixmap::fromImage(im);
        QBrush br(backgroundTexture);
        idf.m_painter->fillRect(idf.m_baseRect, br);
//        QBrush highlightBrush(typeColor);
//        idf.m_painter->fillRect(idf.m_baseRect, highlightBrush);
    }
//    else {
//        if(idf.m_index.data(ROLE_INTERACTION_TYPE)== CatItem::CUSTOM_VERB_INTERACTION){
//            QPen pn;
//            pn.setColor(UI_CUSTOM_VERB_COLOR);
//            idf.m_painter->setPen(pn);
//            idf.m_painter->drawRect(idf.m_baseRect);
//        } else if(idf.m_index.data(ROLE_INTERACTION_TYPE)== CatItem::VERB_ARG_INTERACTION){
//            QPen pn;
//            pn.setColor(UI_CUSTOM_VERB_ARG_COLOR);
//            idf.m_painter->setPen(pn);
//            idf.m_painter->drawRect(idf.m_baseRect);
//        }
//    }
    QPersistentModelIndex hoverIndex =
            ((ListInnerLayout*)(this->parent()))->getRealList()->m_hoveredIndex;
    if(hoverIndex == idf.m_index ){
        idf.m_painter->setPen(idf.m_option.palette.color(QPalette::HighlightedText));
        idf.m_painter->drawRect(idf.m_baseRect);
    }


}

void IconDelegate::setBackground(IconDelegateFormat& idf) const{
    //Background write first
    if (idf.m_option.state & QStyle::State_Selected) {
        idf.m_painter->fillRect(idf.m_baseRect, idf.m_option.palette.highlight());
        idf.m_painter->setPen(idf.m_option.palette.color(QPalette::HighlightedText));
    } else {
        if(idf.m_index.data(ROLE_INTERACTION_TYPE)== CatItem::CUSTOM_VERB_INTERACTION){
            QBrush br;
            br.setColor(UI_CUSTOM_VERB_COLOR);
            idf.m_painter->setBrush(br);

        } else if(idf.m_index.data(ROLE_INTERACTION_TYPE)== CatItem::VERB_ARG_INTERACTION){
            QBrush br;
            br.setColor(UI_CUSTOM_VERB_ARG_COLOR);
            idf.m_painter->setBrush(br);
        }
    }

}

void IconDelegate::drawStandardName(IconDelegateFormat& idf) const{
    //foo/todo - make the rectangles so they correspond to CSS...
    if(idf.m_index.data(ROLE_INTERACTION_TYPE)== CatItem::VERB_ARG_INTERACTION){
        idf.m_nameFont.setPointSize(NAME_FONT_ARG_SIZE);
    } else {
        idf.m_nameFont.setPointSize(NAME_FONT_SIZE_DEFAULT);
    }

    idf.m_baseFontHeight = idf.m_painter->fontMetrics().lineSpacing();
    QFontMetrics nfm(idf.m_nameFont);
    int topFontHeight = nfm.height();
    //int lines = 2;

    //float spaceAvailable = idf.m_baseRect.height() - (topFontHeight + idf.m_baseFontHeight);
    //float innerMargin = UI_LISTITEM_INNER_MARGIN_RATIO*(spaceAvailable/(lines-1));
    //idf.m_outterMargin = nfm.leading();//MAX(1,(spaceAvailable- innerMargin*(lines-1))/2);

    //Draw main text
    idf.m_shortRect = idf.m_baseRect;
    idf.m_shortRect.setHeight(topFontHeight);
    idf.m_shortRect.setLeft(idf.m_baseRect.left() + idf.m_leftTextBorder);
//    idf.m_shortRect.moveTop(idf.m_shortRect.top() + idf.m_outterMargin + topFontHeight*UI_LISTITEM_FONT_ABJ_FACTOR);
    idf.m_shortRect.moveTop(idf.m_shortRect.top() + idf.m_outterMargin );

    QString nameTxt;
    bool altDn = gMainWidget->st_altKeyDown;
    if(altDn){
        nameTxt = idf.m_index.data(ROLE_FORMATTED_SHORT).toString();
    } else {
        nameTxt = idf.m_index.data(ROLE_SHORT).toString();
    }

    idf.m_painter->setFont(idf.m_nameFont);
    idf.m_painter->save();
    QBrush br(Qt::darkGray);
    idf.m_painter->setBrush(br);
    drawHtmlLine(idf.m_painter, idf.m_nameFont, idf.m_shortRect, nameTxt);
    idf.m_painter->restore();
}

void IconDelegate::drawDescription(IconDelegateFormat& idf) const{
    idf.m_longRect = idf.m_baseRect;
    if(idf.m_index.data(ROLE_USE_LONG_DESCRIPTION).toBool()){
        idf.m_longRect.setHeight(idf.m_baseFontHeight*1.5);
    } else {
        idf.m_longRect.setHeight(idf.m_baseFontHeight);
    }

    idf.m_longRect.setHeight(idf.m_baseFontHeight);
    idf.m_longRect.setLeft(idf.m_baseRect.left() + idf.m_leftTextBorder);
    //idf.m_longRect.moveBottom(idf.m_baseRect.bottom() - idf.m_outterMargin);
    idf.m_longRect.moveBottom(idf.m_baseRect.bottom() );

    QString pathText;
    bool altDn = gMainWidget->st_altKeyDown;
    if(altDn){
        pathText = idf.m_index.data(ROLE_FULL_W_ALT).toString();
    } else {
        pathText = idf.m_index.data(ROLE_FULL).toString();
    }
    idf.m_painter->setFont(idf.m_descriptionFont);
    drawHtmlLine(idf.m_painter, idf.m_descriptionFont, idf.m_longRect, pathText,
                 idf.m_index.data(ROLE_USE_LONG_DESCRIPTION).toBool());
}

void IconDelegate::drawTime(IconDelegateFormat& idf) const{
    idf.m_nameFont.setPointSize(TIME_FONT_SIZE_DEFAULT);

    QFontMetrics nfm(idf.m_nameFont);
    int topFontHeight = nfm.lineSpacing();

    //Draw main text
    idf.m_shortRect = idf.m_baseRect;
    idf.m_shortRect.setHeight(topFontHeight);
    idf.m_shortRect.setLeft(idf.m_baseRect.left() + idf.m_leftTextBorder);
    idf.m_shortRect.moveTop(idf.m_shortRect.top() + idf.m_outterMargin );

    QString nameTxt;
    nameTxt = idf.m_index.data(ROLE_TIME_STRING).toString();
    idf.m_painter->setFont(idf.m_nameFont);
    drawHtmlRightAligned(idf.m_painter, idf.m_nameFont, idf.m_shortRect, nameTxt);
}


void IconDelegate::drawSummary(IconDelegateFormat& idf) const{

    //foo/todo - make the rectangles so they correspond to RSS...
    if(idf.m_index.data(ROLE_INTERACTION_TYPE)== CatItem::VERB_ARG_INTERACTION){
        idf.m_nameFont.setPointSize(NAME_FONT_ARG_SIZE);
    }else {
        idf.m_nameFont.setPointSize(NAME_FONT_SIZE_DEFAULT);
    }

    idf.m_baseFontHeight = idf.m_painter->fontMetrics().lineSpacing();
    QFontMetrics nfm(idf.m_nameFont);
    int topFontHeight = nfm.lineSpacing();
//    int lines = 2;
//
//    float spaceAvailable = idf.m_baseRect.height() - (topFontHeight + idf.m_baseFontHeight);
//    float innerMargin = UI_LISTITEM_INNER_MARGIN_RATIO*(spaceAvailable/(lines-1));
    //idf.m_outterMargin = (spaceAvailable- innerMargin*(lines-1))/2;

    //Draw main text
    idf.m_shortRect = idf.m_baseRect;
    idf.m_shortRect.setHeight(topFontHeight);
    idf.m_shortRect.setLeft(idf.m_baseRect.left() + idf.m_leftTextBorder);
    //idf.m_shortRect.moveTop(idf.m_shortRect.top() + idf.m_outterMargin + topFontHeight*UI_LISTITEM_FONT_ABJ_FACTOR);
    QString summaryTxt = idf.m_index.data(ROLE_SUMMARY).toString();
    idf.m_painter->setFont(idf.m_nameFont);
    drawHtmlLine(idf.m_painter, idf.m_nameFont, idf.m_shortRect, summaryTxt, true);

    idf.m_longRect = idf.m_baseRect;
    idf.m_longRect.setLeft(idf.m_baseRect.left() + idf.m_leftTextBorder);
//    idf.m_longRect.moveBottom(idf.m_baseRect.bottom() - idf.m_outterMargin);
    idf.m_longRect.moveBottom(idf.m_baseRect.bottom());
    idf.m_longRect.setHeight(0);

}

void IconDelegate::drawLongName(IconDelegateFormat& idf) const{
    //foo/todo - make the rectangles so they correspond to RSS...
    idf.m_baseFontHeight = idf.m_painter->fontMetrics().lineSpacing();
    if(idf.m_index.data(ROLE_INTERACTION_TYPE)== CatItem::VERB_ARG_INTERACTION){
        idf.m_nameFont.setPointSize(NAME_FONT_ARG_SIZE);
    }else {
        idf.m_nameFont.setPointSize(NAME_FONT_SIZE_DEFAULT);
    }
    //QFontMetrics nfm(idf.m_nameFont);
    //int topFontHeight = nfm.lineSpacing();
    //int lines = 2;

    //float spaceAvailable = idf.m_baseRect.height() - (topFontHeight + idf.m_baseFontHeight);
    //float innerMargin = UI_LISTITEM_INNER_MARGIN_RATIO*(spaceAvailable/(lines-1));
    //idf.m_outterMargin = (spaceAvailable- innerMargin*(lines-1))/2;

    QString nameTxt = idf.m_index.data(ROLE_SHORT).toString();

    //Draw main text
    idf.m_shortRect = idf.m_baseRect;
    //idf.m_shortRect.setHeight(topFontHeight);
    idf.m_shortRect.setLeft(idf.m_baseRect.left() + idf.m_leftTextBorder);
    idf.m_shortRect.moveTop(idf.m_shortRect.top() + idf.m_outterMargin);

    idf.m_painter->setFont(idf.m_nameFont);
    //true = multiline display
    drawHtmlLine(idf.m_painter, idf.m_nameFont, idf.m_shortRect, nameTxt, true);

    idf.m_longRect = idf.m_baseRect;
    idf.m_longRect.setLeft(idf.m_baseRect.left() + idf.m_leftTextBorder);
    idf.m_longRect.moveBottom(idf.m_baseRect.bottom());
    idf.m_longRect.setHeight(0);

}

void IconDelegate::drawArrow(IconDelegateFormat& idf) const{
    //Draw an arrow if the item has a child...
    QRect childIndicatorRect = getChildArrowRect(idf.m_baseRect);
//    childIndicatorRect.setTop(idf.m_baseRect.top() + idf.m_baseRect.height()/4);
//    childIndicatorRect.setHeight(idf.m_baseRect.height()/2);

    MyListWidget* realListPtr = ((ListInnerLayout*)(this->parent()))->getRealList();
    if(!(realListPtr->m_arrowHover && (realListPtr->m_hoveredIndex == idf.m_index))){
        m_arrow_icon.paint(idf.m_painter, childIndicatorRect, Qt::AlignRight | Qt::AlignVCenter);
    } else {
        m_glow_arrow_icon.paint(idf.m_painter, childIndicatorRect,
                           Qt::AlignRight | Qt::AlignVCenter, QIcon::Active,
                           QIcon::On);
    }
}

void IconDelegate::drawNewChildrenCount(IconDelegateFormat& idf) const{
    //Draw an arrow if the item has a child...
    int count = idf.m_index.data(ROLE_NEW_CHILDREN).toInt();
    if(count>0){
        QFont f = idf.m_descriptionFont;
        f.setPixelSize(f.pixelSize()-3);
        QRect pinRect = IconDelegate::getPinItemRect(idf.m_baseRect);
        QRect r(QPoint(pinRect.right(),
                       idf.m_shortRect.top()),
                QPoint(idf.m_baseRect.left(),
                       idf.m_shortRect.bottom()));
        QString countStr = QString::number(count);
        drawHtmlLine(idf.m_painter, f, r, countStr);
    }
}

void IconDelegate::drawMoreItemsIndicator(IconDelegateFormat& idf) const{
    //Draw an arrow if the item has a child...
    QRect childIndicatorRect = idf.m_baseRect;
    childIndicatorRect.setTop(idf.m_baseRect.top() + idf.m_baseRect.height()/4);
    childIndicatorRect.setHeight(idf.m_baseRect.height()/2);
    m_plus_icon.paint(idf.m_painter, childIndicatorRect, Qt::AlignRight | Qt::AlignVCenter);
}

void IconDelegate::drawMiniArrows(IconDelegateFormat& idf) const{
    //Draw an arrow...
    idf.m_painter->save();
    QPersistentModelIndex hoverIndex =
            ((ListInnerLayout*)(this->parent()))->getRealList()->m_hoveredIndex;
    if(idf.m_index.data(ROLE_ITEM_PINNED).toBool()){
            QRect pinRect = IconDelegate::getPinItemRect(idf.m_baseRect);
            m_pinned_already_icon.paint(idf.m_painter, pinRect, Qt::AlignRight | Qt::AlignVCenter);
    } else if(hoverIndex == idf.m_index ){
        QPen pn;
        pn.setColor(Qt::lightGray);
        pn.setWidth(2);
        idf.m_painter->setPen(pn);;
        idf.m_painter->drawRect(idf.m_baseRect);

        QRect pinRect = IconDelegate::getPinItemRect(idf.m_baseRect);
        m_pin_icon.paint(idf.m_painter, pinRect, Qt::AlignRight | Qt::AlignVCenter);
        QRect depricateRect = IconDelegate::getDepricateItemRect(idf.m_baseRect);
        m_depricate_icon.paint(idf.m_painter, depricateRect, Qt::AlignRight | Qt::AlignVCenter);
    }
    idf.m_painter->restore();
}

void IconDelegate::drawBigText(IconDelegateFormat& idf) const {
    //Big Text if any
    int size = idf.m_index.data(ROLE_ITEM_HEIGHT_FACTOR).toInt();
    if(size>1){
        QString longText = idf.m_index.data(ROLE_LONG_TEXT).toString();
        QRect bigRect = idf.m_baseRect;
        bigRect.moveTop(idf.m_shortRect.bottom());
        bigRect.moveBottom(idf.m_longRect.top());
        drawHtmlLine(idf.m_painter, idf.m_descriptionFont, bigRect, longText);
    }
}

void IconDelegate::drawLongBody(IconDelegateFormat& idf) const {
    //Big Text if any
    QString longText = idf.m_index.data(ROLE_LONG_BODY_TEXT).toString();
    QRect bigRect = idf.m_baseRect;
    bigRect.setLeft(idf.m_baseRect.left() + idf.m_leftTextBorder);
    QFont tinyFont = idf.m_descriptionFont;
    tinyFont.setPointSize(tinyFont.pointSize()-2);
    drawHtmlLine(idf.m_painter, idf.m_descriptionFont, bigRect, longText);
}

void IconDelegate::drawDistinguishingRect(IconDelegateFormat& idf) const {
    if((CatItem::DistinguishedType)idf.m_index.data(ROLE_DISTINGUISHED).toInt()
        ==CatItem::HIGH_VALUE){
        idf.m_painter->save();

        QBrush br;
        br.setColor(UI_CUSTOM_HIGH_VALUE_ITEM);
        idf.m_painter->setBrush(br);
        idf.m_painter->drawRect(idf.m_baseRect);

        idf.m_painter->restore();
    }
}



void IconDelegate::paintIcon(IconDelegateFormat& idf) const{

    //Draw Icon
    QRect iconRect = idf.m_option.rect;
    if(idf.m_option.decorationSize.isValid()){
        iconRect.setSize(idf.m_option.decorationSize);
    }
    iconRect.setWidth(UI_STANDARD_ICON_WIDTH);
    iconRect.setHeight(UI_STANDARD_ICON_WIDTH);
    iconRect.setLeft(iconRect.left() + UI_STANDARD_ICON_BORDER/2);
    QIcon icon = idf.m_index.data(ROLE_ICON).value<QIcon>();
    //icon.paint(painter, iconRect);
    QPixmap pixmap = icon.pixmap(iconRect.size(),QIcon::Normal,QIcon::On);
    idf.m_painter->save();
    idf.m_painter->setRenderHint(QPainter::Antialiasing,true);
    idf.m_painter->drawPixmap(iconRect.topLeft(), pixmap);
    idf.m_painter->restore();

}


