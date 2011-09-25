#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QDebug>

#include "regioneditor.h"

static const qreal radius = 5;

class RegionEditorMarker: public QGraphicsEllipseItem
{
public:
  RegionEditorMarker(QGraphicsItem *parent = 0)
    : QGraphicsEllipseItem(-radius, -radius, 2*radius, 2*radius, parent),
      m_boundX(0), m_boundY(0)
  {
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
    setBrush(QColor(240, 200, 40));
    setPen(QPen(Qt::black, 2));
  }

  void bindX(RegionEditorMarker *m) { m_boundX = m; }
  void bindY(RegionEditorMarker *m) { m_boundY = m; }

protected:
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value)
  {
    if (!scene())
      return value;

    RegionEditor *parent = static_cast<RegionEditor *>(parentItem());

    if (change == QGraphicsItem::ItemPositionChange)
    {
      // Constrain movement
      if (parent)
      {
        QSizeF pSize = parent->size();
        QPointF p = value.toPointF();
        // Constrain to area
        qreal xMin = -1;
        qreal xMax = pSize.width()+1;
        qreal yMin = -1;
        qreal yMax = pSize.height()+1;
        // Constrain selection in Y
        if (m_boundX && y() != m_boundX->y())
        {
          if (y()<m_boundX->y())
            yMax = qMin(yMax, m_boundX->y()-3);
          else
            yMin = qMax(yMin, m_boundX->y()+3);
        }
        // Constrain selection in X
        if (m_boundY && x() != m_boundY->x())
        {
          if (x()<m_boundY->x())
            xMax = qMin(xMax, m_boundY->x()-3);
          else
            xMin = qMax(xMin, m_boundY->x()+3);
        }
        return QPointF(qBound(xMin, p.x(), xMax),
                       qBound(yMin, p.y(), yMax));
      }
    }
    else if (change == QGraphicsItem::ItemPositionHasChanged)
    {
      // Update bound markers
      if (m_boundX && x() != m_boundX->x())
        m_boundX->setX(x());

      if (m_boundY && y() != m_boundY->y())
        m_boundY->setY(y());

      if (parent)
        parent->selectionUpdated();
      return value;
    }
  }
private:
  RegionEditorMarker *m_boundX;
  RegionEditorMarker *m_boundY;
};

// =======

RegionEditor::RegionEditor(const QRectF &area, QGraphicsItem *parent)
  : QGraphicsObject(parent), m_selectAll(true)
{
  setFlag(QGraphicsItem::ItemSendsGeometryChanges);

  m_topLeft = new RegionEditorMarker(this);
  m_topRight = new RegionEditorMarker(this);
  m_bottomLeft = new RegionEditorMarker(this);
  m_bottomRight = new RegionEditorMarker(this);

  m_topLeft->bindX(m_bottomLeft);
  m_bottomLeft->bindX(m_topLeft);

  m_topRight->bindX(m_bottomRight);
  m_bottomRight->bindX(m_topRight);

  m_topLeft->bindY(m_topRight);
  m_topRight->bindY(m_topLeft);

  m_bottomLeft->bindY(m_bottomRight);
  m_bottomRight->bindY(m_bottomLeft);

  setArea(area);
}

QRectF RegionEditor::boundingRect() const
{
  return m_area.adjusted(-radius, -radius, radius, radius);
}

void RegionEditor::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  QRectF selected(m_topLeft->pos(), m_bottomRight->pos());
  QRectF inset = selected.adjusted(1, 1, -1, -1);
  QRectF bound = mapRectFromParent(boundingRect());
  QRectF full = mapRectFromParent(m_area);

  painter->setClipRegion(QRegion(bound.toRect())-QRegion(inset.toRect()));
  if (m_showMask)
    painter->fillRect(full, QColor(0, 0, 0, 128));

  painter->setPen(QPen(QColor(0, 0, 0, 128), 2));
  painter->drawRect(selected.toRect());
}

QVariant RegionEditor::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
  return value;
}

void RegionEditor::setArea(const QRectF &area)
{
  m_area = area;
  if (area.size() == QSizeF(0, 0))
    setVisible(false);
  else
  {
    setVisible(true);
    m_topLeft->setVisible(true);
    m_topRight->setVisible(true);
    m_bottomLeft->setVisible(true);
    m_bottomRight->setVisible(true);
  }

  if (m_selectAll)
    resetSelection();
}

QRectF RegionEditor::selection() const
{
  return QRectF(m_topLeft->pos(),
                m_bottomRight->pos()).adjusted(1, 1, -1, -1);
}

void RegionEditor::resetSelection()
{
  m_selectAll = true;
  m_topLeft->setPos(QPointF(-1, -1));
  m_bottomRight->setPos(QPointF(m_area.width()+1, m_area.height()+1));
}

void RegionEditor::selectionUpdated()
{
  m_selectAll = (selection() == m_area);
  update();
}

void RegionEditor::setShowMask(bool value)
{
  m_showMask = value;
  update();
}
