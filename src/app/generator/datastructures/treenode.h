#ifndef TREENODE_H
#define TREENODE_H

#include <QList>

class TreeNode
{
public:
    virtual ~TreeNode() {}
    virtual QList<TreeNode*> getChildren() const { return QList<TreeNode*>(); }
    virtual void addChild(TreeNode* child) { Q_UNUSED(child) }
    virtual TreeNode* getParent() const { return 0; }
    virtual int getType() const = 0;
    virtual QString toString() const = 0;
    virtual void setParent(TreeNode* n) { Q_UNUSED(n) }
};

#endif // TREENODE_H
