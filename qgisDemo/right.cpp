#include "right.h"
#include <QMenu>
#include <QModelIndex>
#include <QIcon>
#include<qgslayertree.h>
#include<qgsmapcanvas.h>
#include<qgsmaplayer.h>
#include <qgslayertreeviewdefaultactions.h>
#include <qgslayertreenode.h>
#include <qgsproject.h>
#include <qgslayertreemodel.h>
#include<qgsrasterlayer.h>
right1::right1(QgsLayerTreeView* view, QgsMapCanvas* canvas)
{
	m_canvas = canvas;
	m_view = view;
}
QMenu* right1::createContextMenu()
{
    QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
	QMenu* menu = new QMenu;

	QgsLayerTreeViewDefaultActions* actions = m_view->defaultActions();

	idx = m_view->currentIndex();
	if (!idx.isValid())
	{
		menu->addAction(actions->actionAddGroup(menu));
		menu->addAction(QIcon(), tr(codec->toUnicode("删除全部").toStdString().c_str()), m_view, SLOT(expandAll()));
	}
    else if (QgsLayerTreeNode* node = m_view->layerTreeModel()->index2node(idx))
    {
        // layer or group selected
        if (QgsLayerTree::isGroup(node))
        {
            menu->addAction(actions->actionZoomToGroup(m_canvas, menu));
           QAction *a=menu->addAction(QIcon(), tr(codec->toUnicode("删除").toStdString().c_str()), QgsProject::instance(), SLOT(removeLayer()));
           connect(a, SIGNAL(triggered()), this, SLOT(removeLayer()));
            menu->addAction(actions->actionRenameGroupOrLayer(menu));
            if (m_view->selectedNodes(true).count() >= 2)
            {
                menu->addAction(actions->actionGroupSelected(menu));
            }
            menu->addAction(actions->actionAddGroup(menu));
        }
        else if (QgsLayerTree::isLayer(node))
        {
             layer = QgsLayerTree::toLayer(node)->layer();
            menu->addAction(actions->actionZoomToLayer(m_canvas, menu));
            //menu->addAction(actions->actionShowInOverview(menu));
            menu->addAction(actions->actionMoveToTop());
            QAction* delete1 = new QAction(QIcon(), tr(codec->toUnicode("删除").toStdString().c_str()), menu);
            menu->addAction(delete1);//, m_view, SLOT(expend(idx)));
            connect(delete1, SIGNAL(triggered()), this, SLOT(deletelayer()));
            QAction* setting2 = new QAction(QIcon(), tr(codec->toUnicode("按种类设色").toStdString().c_str()), menu);
            menu->addAction(setting2);//, m_view, SLOT(expend(idx)));
            connect(setting2, SIGNAL(triggered()), this, SLOT(setlayers()));
            QAction* setting1 = new QAction(QIcon(), tr(codec->toUnicode("设置").toStdString().c_str()), menu);
           menu->addAction(setting1);//this, SLOT(settings(layer)));
           connect(setting1, SIGNAL(triggered()), this, SLOT(settings()));
           if (layer->type() == QgsMapLayerType::RasterLayer)
           {
               QgsRasterLayer* rasterlayer = qobject_cast<QgsRasterLayer*>(layer);
               if (rasterlayer->bandCount() > 1)
               {
                   setting2->setEnabled(false);
               }
               else
               {
                   setting2->setEnabled(true);
               }
           }
           else
           {
               setting2->setEnabled(false);
           }
        }
    }
	return menu;
}
void right1::setlayers()
{
    emit setlayer(layer);
}
void right1::settings()
{
    emit setting(layer);
}
void right1::deletelayer()
{
    emit deletelayers(layer);

}