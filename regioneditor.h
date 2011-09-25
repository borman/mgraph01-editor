#ifndef REGIONEDITOR_H
#define REGIONEDITOR_H

#include <QGraphicsObject>

class RegionEditorMarker;

class RegionEditor : public QGraphicsObject
{
  Q_OBJECT
public:
  RegionEditor(const QRectF &area, QGraphicsItem *parent = 0);

  void setArea(const QRectF &area);

  QRectF selection() const;

  QSizeF size() const { return m_area.size(); }

  // reimplemented
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
  virtual QRectF boundingRect() const;

public slots:
  void resetSelection();
  void setShowMask(bool value);

protected:
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
  void selectionUpdated();

  RegionEditorMarker *m_topLeft;
  RegionEditorMarker *m_topRight;
  RegionEditorMarker *m_bottomLeft;
  RegionEditorMarker *m_bottomRight;
  QRectF m_area;
  bool m_selectAll;
  bool m_showMask;

  friend class RegionEditorMarker;
};

#endif // REGIONEDITOR_H
