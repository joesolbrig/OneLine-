#include <QIcon>
#include "item_rep.h"
#include "cat_builder.h"


ItemRep::ItemRep(CatItem it, Tuple internalId, int change_count,
                 short nthPiece, bool nameFromDescript){
    Q_ASSERT(!it.isEmpty());
    matchType = it.getMatchType();
    matchIndex = it.getMatchIndex();
    //description = it.fullDescription;
    m_internalId = internalId;
    m_name = it.getName();
    Q_ASSERT(!m_name.isEmpty());
    //Q_ASSERT(it.getFullWeight());
    totalWeight = it.getFullWeight();
    change_cn = change_count;
    l_to_match = -1;
    skip_m = false;
    this->itemType = it.getItemType();

    //change_cn = 0;

    m_nthPiece= nthPiece;
    m_name_from_description=nameFromDescript;

}



