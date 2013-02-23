#ifndef AST_H
#define AST_H

#include <QList>
#include "treenode.h"

class AbstractSyntaxTree
{
public:
    AbstractSyntaxTree()
    {
        robotNode = 0;
        moduleNodes = QList<TreeNode*>();
        filterNodes = QList<TreeNode*>();
    }

    ~AbstractSyntaxTree()
    {
        delete robotNode;
        foreach(TreeNode* t, moduleNodes)
            delete t;
        foreach(TreeNode* t, filterNodes)
            delete t;
    }

    TreeNode* getRobotNode() const { return robotNode; }
    QList<TreeNode*> getModuleNodes() const { return moduleNodes; }
    QList<TreeNode*> getFilterNodes() const { return filterNodes; }

    void setRobotNode(TreeNode* n) { robotNode = n; }
    void addModuleNode(TreeNode* n) { moduleNodes << n; }
    void addFilterNode(TreeNode* n) { filterNodes << n; }

private:
    TreeNode* robotNode;
    QList<TreeNode*> moduleNodes;
    QList<TreeNode*> filterNodes;
};

#endif // AST_H
