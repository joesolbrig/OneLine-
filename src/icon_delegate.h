#ifndef ICON_DELEGATE
#define ICON_DELEGATE

#include <QAbstractItemDelegate>
#include "constants.h"
#include "globals.h"
#include "item.h"


#define ROLE_FULL Qt::UserRole+1
#define ROLE_FULL_W_ALT Qt::UserRole+5
#define ROLE_SHORT Qt::UserRole + 10
#define ROLE_FORMATTED_SHORT Qt::UserRole + 15
#define ROLE_ICON Qt::UserRole + 20
#define ROLE_ITEM_ID Qt::UserRole + 30
#define ROLE_ID  Qt::UserRole + 40
#define ROLE_MATCH_TYPE Qt::UserRole + 50
#define ROLE_HAS_CHILDREN Qt::UserRole + 60
#define ROLE_DESCRIPTION Qt::UserRole + 70
#define ROLE_ADJ_PATH Qt::UserRole + 80
#define ROLE_ITEM_TAG_LEVEL Qt::UserRole + 90
#define ROLE_ACTIVE_ICON Qt::UserRole + 100

#define ROLE_ITEM_PATH Qt::UserRole + 110
#define ROLE_ITEM_OFFSET Qt::UserRole + 120
#define ROLE_ITEM_HEIGHT_FACTOR Qt::UserRole + 130
#define ROLE_ITEM_ALT_PATH Qt::UserRole + 140
#define ROLE_LONG_TEXT Qt::UserRole + 150
#define ROLE_BORDER_COLOR Qt::UserRole + 160
#define ROLE_INFO_TEXT Qt::UserRole + 170
#define ROLE_INTERACTION_TYPE Qt::UserRole + 180
#define ROLE_ITEM_PINNED Qt::UserRole + 190
#define ROLE_ITEM_NOT_PINABLE Qt::UserRole + 200

#define ROLE_USE_LONG_NAME Qt::UserRole + 210
#define ROLE_DISTINGUISHED Qt::UserRole + 220
#define ROLE_USE_LONG_DESCRIPTION Qt::UserRole + 230
#define ROLE_USE_ICON Qt::UserRole + 240
#define ROLE_HAS_PARENT_ELEMENT Qt::UserRole + 260
#define ROLE_ITEM_TYPE Qt::UserRole + 270
#define ROLE_TIME_STRING Qt::UserRole + 280
#define ROLE_SUMMARY Qt::UserRole + 290
#define ROLE_NEW_CHILDREN Qt::UserRole + 300
#define ROLE_TOP_MARGIN Qt::UserRole + 310
#define ROLE_LONG_BODY_TEXT Qt::UserRole + 320

class IconDelegate;

struct IconDelegateFormat {
    QPainter *m_painter;
    QModelIndex m_index;
    QStyleOptionViewItem m_option;
    QRect m_baseRect;
    QRect m_shortRect;
    QRect m_longRect;

    QFont m_authorFont;
    QFont m_nameFont;
    QFont m_timeFont;
    QFont m_descriptionFont;


    float m_outterMargin;
    int m_baseFontHeight;
    int m_leftTextBorder;

public:
    IconDelegateFormat(QPainter *painter, QStyleOptionViewItem option, const QModelIndex index);

};

class IconDelegate : public QAbstractItemDelegate
{
	Q_OBJECT

private:
    CatItem m_parentItem;

    QIcon m_arrow_icon;
    QIcon m_glow_arrow_icon;
    QIcon m_plus_icon;
    QIcon m_pin_icon;
    QIcon m_pinned_already_icon;
    QIcon m_depricate_icon;
    QColor m_color;
    QColor m_hicolor;
    QString m_ffamily;
    int m_size;
    int m_fweight;
    int m_isItalics;

    //IconDelegateFormat* m_idf;

public:

    enum DisplayFormat {
        STANDARD =0,
        LONG_NAME,
        POST,
        NEWS_ITEM,
        MESSAGE
    };

    IconDelegate(QObject *parent = 0, CatItem it = CatItem());

    QIcon grayIfy(QIcon icon, int rotate=0);
    QIcon makeGlow(QIcon ic);


    void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;

    QFont delegateFont() const{
        return QFont(m_ffamily, m_size, m_fweight, m_isItalics);
    }

    void setDelegateFont(QFont f) {
        m_ffamily = f.family();
        m_size = f.pointSize();
        m_fweight = f.weight();
        m_isItalics = f.italic();
    }

    QSize sizeHint(const QStyleOptionViewItem& ,
        const QModelIndex& ) const;
    void setColor(QString line, bool hi = false) {
        if (!line.contains(","))
                m_color = QColor(line);

        QStringList spl = line.split(",");
        if (spl.count() != 3) return;
        if (!hi)
            m_color = QColor(spl.at(0).toInt(), spl.at(1).toInt(), spl.at(2).toInt());
        else
            m_hicolor = QColor(spl.at(0).toInt(), spl.at(1).toInt(), spl.at(2).toInt());
    }

    void setFamily(QString fam) { m_ffamily = fam; }
    void setSize(int s) { m_size = s; }
    void setWeight(int w) { m_fweight = w; }
    void setItalics(int i) { m_isItalics = i; }

    //static QRect getDescriptionRectangle();

    static QRect getPinItemRect(QRect containingRect){
        QRect res;
        res.setWidth(UI_LIST_PIN_RECT_WIDTH);
        res.setHeight(UI_LIST_PIN_RECT_HEIGHT);
        res.moveTop(containingRect.top() + UI_LIST_PIN_RECT_TOP_MARGIN);
        res.moveRight(containingRect.right() - UI_LIST_PIN_RECT_RIGHT_MARGIN);
        return res;
    }

    static QRect getDepricateItemRect(QRect containingRect){
        QRect res;
        res.setWidth(UI_LIST_PIN_RECT_WIDTH);
        res.setHeight(UI_LIST_PIN_RECT_HEIGHT);
        res.moveTop(containingRect.top() + UI_LIST_PIN_RECT_TOP_MARGIN);
        res.moveRight(containingRect.right() -
            (UI_LIST_PIN_RECT_RIGHT_MARGIN + UI_LIST_PIN_RECT_WIDTH + UI_LIST_PIN_DEPRICATE_MARGIN));
        return res;
    }

    static QRect getParentArrowRect(QRect containingRect){
        QRect res;
        res.setWidth(UI_LIST_PIN_RECT_WIDTH);
        res.setHeight(UI_LIST_PIN_RECT_HEIGHT);
        res.moveTop(containingRect.top() + UI_LIST_PIN_RECT_TOP_MARGIN);
        res.moveRight(containingRect.right() -
                      (UI_LIST_PIN_RECT_RIGHT_MARGIN + UI_LIST_PIN_RECT_WIDTH + UI_LIST_PIN_DEPRICATE_MARGIN));
        return res;
    }

    static QRect getChildArrowRect(QRect containingRect){
        QRect childIndicatorRect = containingRect;
        childIndicatorRect.setTop(containingRect.top() + containingRect.height()/4);
        childIndicatorRect.setHeight(containingRect.height()/2);
        childIndicatorRect.setLeft(childIndicatorRect.right() - childIndicatorRect.height()*1.5);
        return childIndicatorRect;
    }


private:
    void basicFormat(IconDelegateFormat& idf) const;
    void drawRect(IconDelegateFormat& idf) const;
    void setBackground(IconDelegateFormat& idf) const;
    void drawStandardName(IconDelegateFormat& idf) const;
    void drawDescription(IconDelegateFormat& idf) const;
    void drawLongName(IconDelegateFormat& idf) const;

    void drawTime(IconDelegateFormat& idf) const;
    void drawSummary(IconDelegateFormat& idf) const;

    void drawArrow(IconDelegateFormat& idf) const;
    void drawNewChildrenCount(IconDelegateFormat& idf) const;
    void drawMoreItemsIndicator(IconDelegateFormat& idf) const;
    void drawMiniArrows(IconDelegateFormat& idf) const;
    void drawBigText(IconDelegateFormat& idf) const;
    void drawLongBody(IconDelegateFormat& idf) const;
    void drawDistinguishingRect(IconDelegateFormat& idf) const;
    void paintIcon(IconDelegateFormat& idf) const;

};

#endif
