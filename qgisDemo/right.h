#pragma once
#include <QObject>

#include <qgslayertreeview.h>
#include <qgsmaplayer.h>
#include<qgsmapcanvas.h>
class right1:public QObject, public QgsLayerTreeViewMenuProvider
{
    Q_OBJECT

public:
    right1(QgsLayerTreeView* view, QgsMapCanvas* canvas);
    QMenu* createContextMenu() override;
public slots:
    void settings();
    void deletelayer();
    void setlayers();
signals:
    void setting(QgsMapLayer* layer);
    void setlayer(QgsMapLayer* layer);
    void deletelayers(QgsMapLayer* layer);
private:
    QgsLayerTreeView* m_view = nullptr;
    QgsMapCanvas* m_canvas = nullptr;
    QgsMapLayer* layer;
    QModelIndex idx;
};

