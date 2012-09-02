#include "iconfromurl_reader.h"

IconFromUrlReader::IconFromUrlReader(QObject* parent, CatItem filt): AbstractFeedReader(parent, filt){


}

void IconFromUrlReader::parseTransformedStream(QByteArray transformedData){

    //Create iconfrom raw data
    QPixmap pm;
    pm.loadFromData(transformedData);
    QIcon ic;
    ic.addPixmap(pm);

    //Get path for image cache
    QDir baseDir(PUSER_APP_DIR);
    QString iconPath = baseDir.absoluteFilePath(ICON_SUBDIR_NAME);
    QDir d(iconPath);
    if(!d.exists()){ d.mkdir(iconPath); }
    int id = stringHash(getRequestURL());
    QString iconName = iconPath + PATH_SEP + "netcached" + QString("%1").arg(id);

    //Create thumbnail for image
    QPixmap thumbMap = ic.pixmap(QSize(64,64));
    thumbMap.save(iconName);

    //Connect to item
    m_filt.setOverrideIcon(iconName);
    m_results.append(m_filt);

}
