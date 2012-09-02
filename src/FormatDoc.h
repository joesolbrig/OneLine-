#ifndef FORMATDOC_H
#define FORMATDOC_H

struct FormatRef {
    QTextCursor formatCursor;
    CatItem ref;

};

class FormattedTextDoc : public QTextDocument {
    private:
        QList<FormatRef> formatList;

        CatItem itemAt(QTextCursor posCursor){
            //formatList should be fairly short, otherwise we could quick sort
            //instead here
            for(int i=0;i < formatList.count();i++){
                if(formatList[i+1].formatCursor.position() > posCursor.position()){
                    QTextCursor temp = formatList[i].formatCursor;
                    for(int j=temp.position(); j < posCursor.position()+1;j++ ){


                    }

                }

            }


        }

        void addReference(CatItem it){

        }

        QString referenceString(){

        }

};


#endif // FORMATDOC_H
