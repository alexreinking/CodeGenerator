#include "frcnodes.h"

QString printOpts(OptionsStruct opt)
{
    QString ret = "{";
    QList<QString> keys = opt.keys();
    foreach(QString key, keys) {
        ret += key + ": " + opt.value(key)->toString() + ", ";
    }
    ret.chop(2);
    return ret + "}";
}
