// ************************************************************************** //
//
//  BornAgain: simulate and fit scattering at grazing incidence
//
//! @file      GUI/coregui/Views/SampleDesigner/DesignerScene.cpp
//! @brief     Implements class DesignerScene
//!
//! @homepage  http://www.bornagainproject.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, AUTHORS)
//
// ************************************************************************** //

#include "DesignerScene.h"
#include "ConnectableView.h"
#include "DesignerHelper.h"
#include "DesignerMimeData.h"
#include "IView.h"
#include "LayerView.h"
#include "LocatedItem.h"
#include "NodeEditor.h"
#include "NodeEditorConnection.h"
#include "SampleModel.h"
#include "SampleViewAligner.h"
#include "SampleViewFactory.h"
#include "item_constants.h"
#include "modelmapper.h"
#include "sessionitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QItemSelection>
#include <QPainter>

using namespace ModelView;

DesignerScene::DesignerScene(QObject *parent)
    : QGraphicsScene(parent), m_sampleModel(nullptr),
      m_selectionModel(nullptr), m_proxy(nullptr),
      m_block_selection(false), m_aligner(new SampleViewAligner(this))
{
    setSceneRect(QRectF(-800, 0, 1600, 1600));
    setBackgroundBrush(DesignerHelper::getSceneBackground());

    m_nodeEditor = new NodeEditor(parent);
    m_nodeEditor->install(this);
    connect(m_nodeEditor, &NodeEditor::connectionIsEstablished, this, &DesignerScene::onEstablishedConnection);
    connect(m_nodeEditor, &NodeEditor::selectionModeChangeRequest, this, &DesignerScene::selectionModeChangeRequest);
    connect(this, &DesignerScene::selectionChanged, this, &DesignerScene::onSceneSelectionChanged);
}

DesignerScene::~DesignerScene()
{
    delete m_aligner;
}

void DesignerScene::setSampleModel(SampleModel *sampleModel)
{
    Q_ASSERT(sampleModel);

    if (sampleModel == m_sampleModel)
        return;

    if (m_sampleModel)
        m_sampleModel->mapper()->unsubscribe(this);

    m_sampleModel = sampleModel;
    m_sampleModel->mapper()->setOnModelReset([this](SessionModel*) { resetScene(); }, this);

    m_sampleModel->mapper()->setOnRowInserted(
        [this](SessionItem*, std::string, int) { onRowsInserted(); }, this);

    m_sampleModel->mapper()->setOnRowRemoved(
        [this](SessionItem*, std::string, int) { onRowsRemoved(); }, this);

    resetScene();
    updateScene();
}

void DesignerScene::setSelectionModel(QItemSelectionModel *model, FilterPropertyProxy *proxy)
{
    Q_ASSERT(model);

    if (model != m_selectionModel) {

        if (m_selectionModel) {
            disconnect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
                       this, SLOT(onSessionSelectionChanged(QItemSelection, QItemSelection)));
        }

        m_selectionModel = model;
        m_proxy = proxy;

        connect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this,
                SLOT(onSessionSelectionChanged(QItemSelection, QItemSelection)));
    }
}

IView* DesignerScene::getViewForItem(SessionItem* item)
{
    auto it = m_ItemToView.find(item);
    return it != m_ItemToView.end() ? it.value() : nullptr;
}

void DesignerScene::resetScene()
{
    clear();
    m_ItemToView.clear();
    m_layer_interface_line = QLineF();
}

void DesignerScene::updateScene()
{
    updateViews();
    alignViews();
}

void DesignerScene::onRowsInserted()
{
    deleteViews();
    updateScene();
}

void DesignerScene::onRowsRemoved()
{
    deleteViews();
    updateScene();
}

//! propagate selection from model to scene
void DesignerScene::onSessionSelectionChanged(const QItemSelection & /* selected */,
                                              const QItemSelection & /* deselected */)
{
    /*if (m_block_selection)
        return;

    m_block_selection = true;

    for (QMap<SessionItem *, IView *>::iterator it = m_ItemToView.begin();
         it != m_ItemToView.end(); ++it) {
        QModelIndex index = m_proxy->mapFromSource(m_sampleModel->indexOfItem(it.key()));
        if (index.isValid()) {
            it.value()->setSelected(m_selectionModel->isSelected(index));
        }
    }

    m_block_selection = false;*/
}

//! propagate selection from scene to model
void DesignerScene::onSceneSelectionChanged()
{
    /*if (m_block_selection)
        return;

    m_block_selection = true;

    m_selectionModel->clearSelection();
    QList<QGraphicsItem *> selected = selectedItems();
    for (int i = 0; i < selected.size(); ++i) {
        IView *view = dynamic_cast<IView *>(selected[i]);
        if (view) {
            SessionItem *sampleItem = view->getItem();
            QModelIndex itemIndex = m_sampleModel->indexOfItem(sampleItem);
            Q_ASSERT(itemIndex.isValid());
            if (!m_selectionModel->isSelected(m_proxy->mapFromSource(itemIndex)))
                m_selectionModel->select(m_proxy->mapFromSource(itemIndex), QItemSelectionModel::Select);
        }
    }

    m_block_selection = false;*/
}

//! runs through all items recursively and updates corresponding views
void DesignerScene::updateViews()
{
    if (!m_sampleModel) {
        Q_ASSERT(m_sampleModel);
        return;
    }

    QList<SessionItem*> to_process;
    for (auto item : m_sampleModel->rootItem()->children())
        to_process.append(item);

    while (!to_process.empty()) {
        auto item = to_process.takeFirst();
        const auto children = item->children();
        std::for_each(children.rbegin(), children.rend(), [&to_process](auto item) {
            if (item && SampleViewFactory::isValidType(item->modelType()))
                to_process.push_front(item);
        });

        addViewForItem(item);
    }
}

//! adds view for item, if it doesn't exists
void DesignerScene::addViewForItem(SessionItem *item)
{
    if (!item) {
        Q_ASSERT(item);
        return;
    }

    IView* view = getViewForItem(item);
    if (view) // view for the item already exists
        return;

    view = SampleViewFactory::createSampleView(item->modelType());
    if (!view)
        return;

    m_ItemToView[item] = view;
    view->setParameterizedItem(item);

    if (auto parent_view = getViewForItem(item->parent()))
        parent_view->addView(view);
    else
        addItem(view);
}

//! aligns SampleView's on graphical canvas
void DesignerScene::alignViews()
{
    m_aligner->alignSample(m_sampleModel->rootItem(), QPointF(200, 800));
}

//! runs recursively through model's item and schedules view removal
void DesignerScene::deleteViews()
{
    for (auto view: m_ItemToView.values()) {
        view->setSelected(false);
        emit view->aboutToBeDeleted();
        view->deleteLater();
    }
    m_ItemToView.clear();
    update();
}

//! propagates deletion of views on the scene to the model
void DesignerScene::deleteSelectedItems()
{
    /*QModelIndexList indexes = m_selectionModel->selectedIndexes();

    QList<IView *> views_which_will_be_deleted;
    for(auto index : indexes) {
        views_which_will_be_deleted.append(m_ItemToView[m_sampleModel->itemForIndex(m_proxy->mapToSource(index))]);
    }
    // deleting selected items on model side, corresponding views will be deleted automatically
    // Since we don't know the order of items and their parent/child relationship, we need this
    while (indexes.size()) {
        QModelIndex current = m_proxy->mapToSource(indexes.back());
        m_sampleModel->removeRows(current.row(), 1, current.parent());
        indexes = m_selectionModel->selectedIndexes();
    }
    // Connections will be deleted automatically if one of connected views has been deleted.
    // For the moment, we have to delete connections which are: 1) were selected 2) Do not connect
    // views scheduled for deletion.
    for(auto graphicsItem : selectedItems()) {
        if (NodeEditorConnection *connection = dynamic_cast<NodeEditorConnection *>(graphicsItem)) {
            if (views_which_will_be_deleted.contains(connection->getParentView())
                || views_which_will_be_deleted.contains(connection->getChildView()))
                continue;
            removeConnection(connection);
        }
    }*/
}

//! shows appropriate layer interface to drop while moving ILayerView
void DesignerScene::drawForeground(QPainter *painter, const QRectF & /* rect */)
{
    if (isLayerDragged()) {
        painter->setPen(QPen(Qt::darkBlue, 2, Qt::DashLine));
        painter->drawLine(m_layer_interface_line);
    }
}

//! propagates connection established by NodeEditor to the model
void DesignerScene::onEstablishedConnection(NodeEditorConnection *connection)
{
    /*ConnectableView *parentView = connection->getParentView();
    ConnectableView *childView = connection->getChildView();

    QString tag;
    delete connection; // deleting just created connection because it will be recreated from the
                       // model
    m_sampleModel->moveItem(childView->getItem(),
                                         parentView->getItem(), -1, tag);*/
}

//! propagates break of connection between views on scene to the model
void DesignerScene::removeConnection(NodeEditorConnection *connection)
{
    /*IView *childView = dynamic_cast<IView *>(connection->outputPort()->parentItem());
    m_sampleModel->moveItem(childView->getItem(), 0);*/
}

//! handles drag event
//! LayerView can be dragged only over MultiLayerView
//! MultiLayerView can be dragged both, over the scene and over another MultiLayerView
void DesignerScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    const DesignerMimeData *mimeData = checkDragEvent(event);
    if(isAcceptedByMultiLayer(mimeData, event)) {
        QGraphicsScene::dragMoveEvent(event);

    }
}

//! Hadles drop event
//! LayerView can be dropped on MultiLayerView only
//! MultiLayerView can be droped on the scene or another MultiLayerView
void DesignerScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    const DesignerMimeData *mimeData = checkDragEvent(event);
    if (mimeData) {

        // to have possibility to drop MultiLayer on another MultiLayer
        // * edit function DesignerScene::isAcceptedByMultiLayer
        // * edit MultiLayerItem for addToValidChildren
        // * remove method MultiLayerView::itemChange

        if(isAcceptedByMultiLayer(mimeData, event)) {
            // certain views can be dropped on MultiLayer and so will be processed there
            QGraphicsScene::dropEvent(event);

        } else {
            // other views can be dropped on canvas anywhere
            if (SampleViewFactory::isValidType(mimeData->getClassName())) {

                SessionItem *new_item = m_sampleModel->insertNewItem(mimeData->getClassName());

                // propagating drop coordinates to SessionItem
                QRectF boundingRect = DesignerHelper::getDefaultBoundingRect(new_item->modelType());
                new_item->getItem(LocatedItem::P_X_POS)->setData(event->scenePos().x() - boundingRect.width() / 2, ItemDataRole::DATA);
                new_item->getItem(LocatedItem::P_Y_POS)->setData(event->scenePos().y() - boundingRect.height() / 2, ItemDataRole::DATA);

            }
            adjustSceneRect();
        }
    }
}

//! returns proper MimeData if the object can be hadled by graphics scene
const DesignerMimeData *DesignerScene::checkDragEvent(QGraphicsSceneDragDropEvent *event)
{
    const DesignerMimeData *mimeData = qobject_cast<const DesignerMimeData *>(event->mimeData());
    if (!mimeData) {
        event->ignore();
        return nullptr;
    }
    event->setAccepted(true);
    return mimeData;
}

void DesignerScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(isLayerDragged()) {
        invalidate(); // to redraw vertical dashed line which denotes where to drag the layer
    }
    QGraphicsScene::mouseMoveEvent(event);
}

//! Returns true if there is MultiLayerView nearby during drag event.
bool DesignerScene::isMultiLayerNearby(QGraphicsSceneDragDropEvent *event)
{
    QRectF rect = DesignerHelper::getDefaultMultiLayerRect();
    rect.moveCenter(event->scenePos());
    for(QGraphicsItem* item : items(rect)) {
        if (item->type() == DesignerHelper::MULTILAYER)
            return true;
    }
    return false;
}

void DesignerScene::adjustSceneRect()
{
    QRectF boundingRect = itemsBoundingRect();
    if (sceneRect().contains(boundingRect))
        return;

    boundingRect.adjust(20.0, 20.0, 20.0, 20.0);
    setSceneRect(sceneRect().united(boundingRect));
}

bool DesignerScene::isAcceptedByMultiLayer(const DesignerMimeData *mimeData, QGraphicsSceneDragDropEvent *event)
{
    if(!mimeData) return false;

    // MultiLayer can be inserted in MultiLayer
    if (mimeData->getClassName() == ::Constants::MultiLayerType && isMultiLayerNearby(event)) {
        return true;
    }

    // layer can be inserted in MultiLayer
    if (mimeData->getClassName() == ::Constants::LayerType && isMultiLayerNearby(event)) {
        return true;
    }
    return false;
}

bool DesignerScene::isLayerDragged() const
{
    ILayerView *layer = dynamic_cast<ILayerView *>(mouseGrabberItem());
    return layer && !m_layer_interface_line.isNull();
}

void DesignerScene::onSmartAlign()
{
    m_aligner->smartAlign();
}