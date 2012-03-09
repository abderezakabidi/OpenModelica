/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Linkoping University,
 * Department of Computer and Information Science,
 * SE-58183 Linkoping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 
 * AND THIS OSMC PUBLIC LICENSE (OSMC-PL). 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S  
 * ACCEPTANCE OF THE OSMC PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Linkoping University, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main Authors 2010: Syed Adeel Asghar, Sonia Tariq
 * Contributors 2011: Abhinn Kothari
 */

#include "ShapeAnnotation.h"
#include "ProjectTabWidget.h"

ShapeAnnotation::ShapeAnnotation(QGraphicsItem *parent)
    : QGraphicsItem(parent), mSettings(QSettings::IniFormat, QSettings::UserScope, "openmodelica", "omedit")
{
    mpGraphicsView = 0;
}

ShapeAnnotation::ShapeAnnotation(GraphicsView *graphicsView, QGraphicsItem *parent)
    : QGraphicsItem(parent), mSettings(QSettings::IniFormat, QSettings::UserScope, "openmodelica", "omedit")
{
    mpGraphicsView = graphicsView;
    createActions();
}

ShapeAnnotation::~ShapeAnnotation()
{
    // delete all the corner items associated with item
    foreach (RectangleCornerItem *rectangleCornerItem, mRectangleCornerItemsList)
    {
        delete rectangleCornerItem;
    }
}

void ShapeAnnotation::initializeFields()
{
    // initialize the Line Patterns map.
    this->mLinePatternsMap.insert("None", Qt::NoPen);
    this->mLinePatternsMap.insert("Solid", Qt::SolidLine);
    this->mLinePatternsMap.insert("Dash", Qt::DashLine);
    this->mLinePatternsMap.insert("Dot", Qt::DotLine);
    this->mLinePatternsMap.insert("DashDot", Qt::DashDotLine);
    this->mLinePatternsMap.insert("DashDotDot", Qt::DashDotDotLine);
    // initialize the Fill Patterns map.
    this->mFillPatternsMap.insert("None", Qt::NoBrush);
    this->mFillPatternsMap.insert("Solid", Qt::SolidPattern);
    this->mFillPatternsMap.insert("Horizontal", Qt::HorPattern);
    this->mFillPatternsMap.insert("Vertical", Qt::VerPattern);
    this->mFillPatternsMap.insert("Cross", Qt::CrossPattern);
    // since we flipped the co-ordinates of view so to get patterns right switch between forward and backward.
    this->mFillPatternsMap.insert("Forward", Qt::BDiagPattern);
    this->mFillPatternsMap.insert("Backward", Qt::FDiagPattern);
    this->mFillPatternsMap.insert("CrossDiag", Qt::DiagCrossPattern);
    this->mFillPatternsMap.insert("HorizontalCylinder", Qt::LinearGradientPattern);
    this->mFillPatternsMap.insert("VerticalCylinder", Qt::Dense1Pattern);
    this->mFillPatternsMap.insert("Sphere", Qt::RadialGradientPattern);

    // initialize the Arrows map.
    this->mArrowsMap.insert("None", ShapeAnnotation::None);
    this->mArrowsMap.insert("Open", ShapeAnnotation::Open);
    this->mArrowsMap.insert("Filled", ShapeAnnotation::Filled);
    this->mArrowsMap.insert("Half", ShapeAnnotation::Half);

    this->mVisible = true;
    mOrigin.setX(0);
    mOrigin.setY(0);
    mRotation = 0;
     //loads the initial saved settings for the shape
    mLineColor=QColor(0,0,255);
    this->mFillColor = QColor (0, 0, 255);
    mLinePattern = Qt::SolidLine;
    this->mFillPattern = Qt::NoBrush;
    mThickness = 0.25;
    this->mBorderPattern = Qt::NoBrush;
    this->mCornerRadius = 0;
    mSmooth = false;
    mStartArrow = ShapeAnnotation::None;
    mEndArrow = ShapeAnnotation::None;
    mArrowSize = 3;

    mIsCustomShape = false;
    mIsFinishedCreatingShape = false;
    mIsRectangleCorneItemClicked = false;
    readSettings();
}


void ShapeAnnotation::readSettings()
{
    mSettings.sync();
    readPenStyleSettings();
    readBrushStyleSettings();
}



void ShapeAnnotation::readPenStyleSettings()
{
    if (mSettings.contains("penstyle/color"))
    {
        if (mSettings.value("penstyle/color").toString().isEmpty())
            mLineColor=Qt::transparent;
        else
            mLineColor= QColor(mSettings.value("penstyle/color").toUInt());
    }
    if (mSettings.contains("penstyle/pattern"))
          mLinePattern = mLinePatternsMap.value(mSettings.value("penstyle/pattern").toString());
        //  mLinePattern=Qt::PenStyle(mSettings.value("penstyle/pattern"));
    if (mSettings.contains("penstyle/thickness"))
      mThickness=mSettings.value("penstyle/thickness").toDouble();
    if (mSettings.contains("penstyle/smooth"))
     mSmooth= mSettings.value("penstyle/smooth").toBool();
}

void ShapeAnnotation::readBrushStyleSettings()
{
    if (mSettings.contains("brushstyle/color"))
    {
        if (mSettings.value("brushstyle/color").toString().isEmpty())
            mFillColor= Qt::transparent;
        else
           mFillColor= QColor(mSettings.value("brushstyle/color").toUInt());
    }
    if (mSettings.contains("brushstyle/pattern"))
    mFillPattern = mFillPatternsMap.value(mSettings.value("brushstyle/pattern").toString());
    if(mFillColor==Qt::transparent)
    mFillPattern= Qt::NoBrush;
}

void ShapeAnnotation::createActions()
{
    mpShapePropertiesAction = new QAction(QIcon(":/Resources/icons/tool.png"), tr("Properties"), mpGraphicsView);
    mpShapePropertiesAction->setStatusTip(tr("Shows the shape properties"));
    connect(mpShapePropertiesAction, SIGNAL(triggered()), SLOT(openShapeProperties()));
    //for editing properties of text
    mpTextPropertiesAction = new QAction(QIcon(":/Resources/icons/tool.png"), tr("Text Properties"), mpGraphicsView);
    mpTextPropertiesAction->setStatusTip(tr("Shows the shape text properties"));
    connect(mpTextPropertiesAction, SIGNAL(triggered()), SLOT(openTextProperties()));
    //for arrow of line
    mpNoArrowAction = new QAction(tr("NoArrow(Start)"), mpGraphicsView);
    mpNoArrowAction->setStatusTip(tr("No Arrow on the line"));
    connect(mpNoArrowAction, SIGNAL(triggered()), SLOT(noArrowLine()));
    mpHalfArrowAction = new QAction(tr("HalfArrow(Start)"), mpGraphicsView);
    mpHalfArrowAction->setStatusTip(tr("Half Arrow for the line"));
    connect(mpHalfArrowAction, SIGNAL(triggered()), SLOT(halfArrowLine()));
    mpOpenArrowAction = new QAction(tr("OpenArrow(Start)"), mpGraphicsView);
    mpOpenArrowAction->setStatusTip(tr("Open Arrow on the line"));
    connect(mpOpenArrowAction, SIGNAL(triggered()), SLOT(openArrowLine()));
    mpFilledArrowAction = new QAction(tr("FilledArrow(Start)"), mpGraphicsView);
    mpFilledArrowAction->setStatusTip(tr("Filled Arrow for the line"));
    connect(mpFilledArrowAction, SIGNAL(triggered()), SLOT(filledArrowLine()));
    mpNoEndArrowAction = new QAction(tr("NoArrow(End)"), mpGraphicsView);
    mpNoEndArrowAction->setStatusTip(tr("No Arrow on the line"));
    connect(mpNoEndArrowAction, SIGNAL(triggered()), SLOT(noEndArrowLine()));
    mpHalfEndArrowAction = new QAction(tr("HalfArrow(End)"), mpGraphicsView);
    mpHalfEndArrowAction->setStatusTip(tr("Half Arrow for the line"));
    connect(mpHalfEndArrowAction, SIGNAL(triggered()), SLOT(halfEndArrowLine()));
    mpOpenEndArrowAction = new QAction(tr("OpenArrow(End)"), mpGraphicsView);
    mpOpenEndArrowAction->setStatusTip(tr("Open Arrow on the line"));
    connect(mpOpenEndArrowAction, SIGNAL(triggered()), SLOT(openEndArrowLine()));
    mpFilledEndArrowAction = new QAction(tr("FilledArrow(End"), mpGraphicsView);
    mpFilledEndArrowAction->setStatusTip(tr("Filled Arrow for the line"));
    connect(mpFilledEndArrowAction, SIGNAL(triggered()), SLOT(filledEndArrowLine()));

}

void ShapeAnnotation::setSelectionBoxActive()
{
    foreach (RectangleCornerItem *rectangleCornerItem, mRectangleCornerItemsList)
    {
        rectangleCornerItem->setActive();
    }
}

void ShapeAnnotation::setSelectionBoxPassive()
{
    foreach (RectangleCornerItem *rectangleCornerItem, mRectangleCornerItemsList)
    {
        rectangleCornerItem->setPassive();
    }
}

void ShapeAnnotation::setSelectionBoxHover()
{
    foreach (RectangleCornerItem *rectangleCornerItem, mRectangleCornerItemsList)
    {
        rectangleCornerItem->setHovered();
    }
}

QString ShapeAnnotation::getShapeAnnotation()
{
    return QString();
}

QRectF ShapeAnnotation::getBoundingRect() const
{
    QPointF p1 = mExtent.size() > 0 ? mExtent.at(0) : QPointF(-100.0, -100.0);
    QPointF p2 = mExtent.size() > 1 ? mExtent.at(1) : QPointF(100.0, 100.0);

    qreal left = qMin(p1.x(), p2.x());
    qreal top = qMin(p1.y(), p2.y());
    qreal width = fabs(p1.x() - p2.x());
    qreal height = fabs(p1.y() - p2.y());

    return QRectF (left, top, width, height);
}

QPainterPath ShapeAnnotation::addPathStroker(QPainterPath &path) const
{
    QPainterPathStroker stroker;
    stroker.setWidth(Helper::shapesStrokeWidth);
    return stroker.createStroke(path);
}

//! Tells the component to ask its parent to delete it.
void ShapeAnnotation::deleteMe()
{
    // make sure you disconnect all signals before deleting the object
    disconnect(mpGraphicsView->mpHorizontalFlipAction, SIGNAL(triggered()), this, SLOT(flipHorizontal()));
    disconnect(mpGraphicsView->mpVerticalFlipAction, SIGNAL(triggered()), this, SLOT(flipVertical()));
    disconnect(mpGraphicsView->mpRotateIconAction, SIGNAL(triggered()), this, SLOT(rotateClockwise()));
    disconnect(mpGraphicsView->mpRotateAntiIconAction, SIGNAL(triggered()), this, SLOT(rotateAntiClockwise()));
    disconnect(mpGraphicsView->mpResetRotation, SIGNAL(triggered()), this, SLOT(resetRotation()));
    disconnect(mpGraphicsView->mpDeleteIconAction, SIGNAL(triggered()), this, SLOT(deleteMe()));
    disconnect(mpGraphicsView, SIGNAL(keyPressDelete()), this, SLOT(deleteMe()));
    disconnect(mpGraphicsView, SIGNAL(keyPressUp()), this, SLOT(moveUp()));
    disconnect(mpGraphicsView, SIGNAL(keyPressDown()), this, SLOT(moveDown()));
    disconnect(mpGraphicsView, SIGNAL(keyPressLeft()), this, SLOT(moveLeft()));
    disconnect(mpGraphicsView, SIGNAL(keyPressRight()), this, SLOT(moveRight()));
    disconnect(mpGraphicsView, SIGNAL(keyPressRotateClockwise()), this, SLOT(rotateClockwise()));
    disconnect(mpGraphicsView, SIGNAL(keyPressRotateAntiClockwise()), this, SLOT(rotateAntiClockwise()));
    // delete the object
    GraphicsView *pGraphicsView = qobject_cast<GraphicsView*>(const_cast<QObject*>(sender()));
    mpGraphicsView->deleteShapeObject(this);
    mpGraphicsView->scene()->removeItem(this);
    // if the signal is not send by graphicsview then emit updateshapeannotation
    if (!pGraphicsView)
    {
        emit updateShapeAnnotation();
    }
    deleteLater();
}

void ShapeAnnotation::doSelect()
{
    mIsRectangleCorneItemClicked = true;
    if (!this->isSelected())
        setSelectionBoxActive();
}

void ShapeAnnotation::doUnSelect()
{
    mIsRectangleCorneItemClicked = false;
    if (!this->isSelected())
        setSelectionBoxPassive();
}

//! Slot that moves component one pixel upwards
//! @see moveDown()
//! @see moveLeft()
//! @see moveRight()
void ShapeAnnotation::moveUp()
{
    //qDebug() << "in update R22:: " <<mExtent<<mapFromScene(mapToScene(this->pos()));
   // QPointF point = mExtent.at(0);
   // point.ry()++;
   // mExtent.replace(0, point);
   // point = mExtent.at(1);
   // point.ry()++;
   // mExtent.replace(1, point);
    //qDebug() << "in update R33:: " <<mExtent<<mapFromScene(mapToScene(this->pos()));
    this->setPos(this->pos().x(), this->mapFromScene(this->mapToScene(this->pos())).y()+1);
    mpGraphicsView->scene()->update();

    //qDebug() << "in update R11:: " <<mExtent.at(0);
    emit updateShapeAnnotation();
    //qDebug() << "in update R22:: " <<mExtent.at(0);
    //mExtent.replace(0,mapToScene(mExtent.at(0)));
    //mExtent.replace(1,mapToScene(mExtent.at(1)));
    //qDebug() << "in update R33:: " <<mExtent.at(0);
}

//! Slot that moves component one pixel downwards
//! @see moveUp()
//! @see moveLeft()
//! @see moveRight()
void ShapeAnnotation::moveDown()
{
    this->setPos(this->pos().x(), this->mapFromScene(this->mapToScene(this->pos())).y()-1);
    mpGraphicsView->scene()->update();
    emit updateShapeAnnotation();

    //mExtent.replace(0,mapToScene(mExtent.at(0)));
    //mExtent.replace(1,mapToScene(mExtent.at(1)));
}

//! Slot that moves component one pixel leftwards
//! @see moveUp()
//! @see moveDown()
//! @see moveRight()
void ShapeAnnotation::moveLeft()
{
    this->setPos(this->mapFromScene(this->mapToScene(this->pos())).x()-1, this->pos().y());
    mpGraphicsView->scene()->update();
    emit updateShapeAnnotation();
    //mExtent.replace(0,mapToScene(mExtent.at(0)));
    //mExtent.replace(1,mapToScene(mExtent.at(1)));
}

//! Slot that moves component one pixel rightwards
//! @see moveUp()
//! @see moveDown()
//! @see moveLeft()
void ShapeAnnotation::moveRight()
{
    this->setPos(this->mapFromScene(this->mapToScene(this->pos())).x()+1, this->pos().y());
    mpGraphicsView->scene()->update();
    emit updateShapeAnnotation();
   // mExtent.replace(0,mapToScene(mExtent.at(0)));
   // mExtent.replace(1,mapToScene(mExtent.at(1)));
}

void ShapeAnnotation::rotateClockwise()
{
    //qDebug() << "in update before rot:: " <<mExtent;
    qreal rotation = this->rotation();
    qreal rotateIncrement = -90;

    if (rotation == -270)
        this->setRotation(0);
    else
        this->setRotation(rotation + rotateIncrement);

    mpGraphicsView->scene()->update();
    // if not component instance then updateshapeannotation, component handles this in his own class
    if (!dynamic_cast<Component*>(this))
        emit updateShapeAnnotation();
   // qDebug() << "in update after rot:: " <<mExtent;
    //mExtent.replace(0,mapFromScene(this->mRectangleCornerItemsList.at(0)->pos()));
    //mExtent.replace(1,mapFromScene(this->mRectangleCornerItemsList.at(1)->pos()));
    //qDebug() << "in update corner pos after replace:: " <<this->mRectangleCornerItemsList.at(0)->pos();

//qDebug() << "in update :: " <<mExtent;
//mpGraphicsView->scene()->update();
}
//currently not working correctly
void ShapeAnnotation::flipHorizontal()
{
    QRectF rectan = this->boundingRect();
    qreal lft =rectan.left();
    qreal rght = rectan.right();
    qreal trans =0- lft-rght;
    this->scale(-1,1);
    this->translate(trans,0);
    emit updateShapeAnnotation();
}
//currently not working correctly
void ShapeAnnotation::flipVertical()
{
    QRectF rectan = this->boundingRect();
    qreal tp =rectan.top();
    qreal btm = rectan.bottom();
    qreal trans =0- tp-btm;
    this->scale(1,-1);
    this->translate(0,trans);

    emit updateShapeAnnotation();
}

void ShapeAnnotation::rotateAntiClockwise()
{
    qreal rotation = this->rotation();
    qreal rotateIncrement = 90;
    if (rotation == 270)
        this->setRotation(0);
    else
        this->setRotation(rotation + rotateIncrement);

    // if not component instance then updateshapeannotation, component handles this in his own class
    if (!dynamic_cast<Component*>(this))
        emit updateShapeAnnotation();
}

void ShapeAnnotation::resetRotation()
{
    this->setRotation(0);
    // if not component instance then updateshapeannotation, component handles this in his own class
    if (!dynamic_cast<Component*>(this))
        emit updateShapeAnnotation();
}
//pen stlye changed for the selected shape shape
void ShapeAnnotation::changePenProperty()
{
    this->mLineColor = mpShapeProperties->getPenColor().rgba();
    this->mLinePattern = mpShapeProperties->getPenPattern();
    this->mThickness = mpShapeProperties->getPenThickness();
    this->mCornerRadius = mpShapeProperties->getCornerRadius();
    this->mSmooth = mpShapeProperties->getPenSmooth();
}

//brush style changed for the selected shape
void ShapeAnnotation::changeBrushProperty()
{
    this->mFillColor=mpShapeProperties->getBrushColor();
    this->mFillPattern=mpShapeProperties->getBrushPattern();
}

void ShapeAnnotation::openShapeProperties()
{
    mpShapeProperties = new ShapeProperties(this, mpGraphicsView->mpParentProjectTab->mpParentProjectTabWidget->mpParentMainWindow);
    mpShapeProperties->show();
}

void ShapeAnnotation::openTextProperties()
{
    mpGraphicsView->mpTextWidget = new TextWidget(mpGraphicsView->mpTextShape, mpGraphicsView->mpParentProjectTab->mpParentProjectTabWidget->mpParentMainWindow);
    mpGraphicsView->mpTextWidget->show();
}


QColor ShapeAnnotation::getLineColor()
{
    return mLineColor;
}

Qt::PenStyle ShapeAnnotation::getLinePattern()
{
    return mLinePattern;
}

double ShapeAnnotation::getLineThickness()
{
    return mThickness;
}

double ShapeAnnotation::getRectCornerRadius()
{
    return mCornerRadius;
}

bool ShapeAnnotation::getLineSmooth()
{
    return mSmooth;
}

QColor ShapeAnnotation::getFillColor()
{
    return mFillColor;
}

Qt::BrushStyle ShapeAnnotation::getFillPattern()
{
    return mFillPattern;
}

void ShapeAnnotation::noArrowLine()
{
  this->mStartArrow = ShapeAnnotation::None;
}

void ShapeAnnotation::halfArrowLine()
{
  this->mStartArrow = ShapeAnnotation::Half;
}

void ShapeAnnotation::openArrowLine()
{
  this->mStartArrow = ShapeAnnotation::Open;
}

void ShapeAnnotation::filledArrowLine()
{
  this->mStartArrow = ShapeAnnotation::Filled;
}

void ShapeAnnotation::noEndArrowLine()
{
  this->mEndArrow = ShapeAnnotation::None;
}

void ShapeAnnotation::halfEndArrowLine()
{
  this->mEndArrow = ShapeAnnotation::Half;
}

void ShapeAnnotation::openEndArrowLine()
{
  this->mEndArrow = ShapeAnnotation::Open;
}

void ShapeAnnotation::filledEndArrowLine()
{
  this->mEndArrow = ShapeAnnotation::Filled;
}


//! Event when mouse cursor enters component icon.
void ShapeAnnotation::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    // only use hover events for user defined lines
    if (!mIsCustomShape)
        return;

    if(!this->isSelected())
        setSelectionBoxHover();
}

//! Event when mouse cursor leaves component icon.
void ShapeAnnotation::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);

    // only use hover events for user defined lines
    if (!mIsCustomShape)
        return;

    if(!this->isSelected())
        setSelectionBoxPassive();
}

void ShapeAnnotation::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;
    // only use mouse events for user defined lines
    if (!mIsCustomShape or !mIsFinishedCreatingShape)
    {
        QGraphicsItem::mousePressEvent(event);
        return;
    }

    mClickPos = mapToScene(event->pos());
    mIsItemClicked = true;
    setCursor(Qt::SizeAllCursor);
    QGraphicsItem::mousePressEvent(event);
}

void ShapeAnnotation::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // only use mouse events for user defined lines
    if (!mIsCustomShape or !mIsFinishedCreatingShape)
    {
        QGraphicsItem::mouseReleaseEvent(event);
        return;
    }

    if (mClickPos != mapToScene(event->pos()))
    {
        mIsItemClicked = false;
        emit updateShapeAnnotation();
    }
    unsetCursor();
    QGraphicsItem::mouseReleaseEvent(event);
}

QVariant ShapeAnnotation::itemChange(GraphicsItemChange change, const QVariant &value)
{
    QGraphicsItem::itemChange(change, value);
    if (change == QGraphicsItem::ItemSelectedHasChanged)
    {
        if (this->isSelected())
        {
            setSelectionBoxActive();
            // make the connections now
            connect(mpGraphicsView->mpHorizontalFlipAction, SIGNAL(triggered()), SLOT(flipHorizontal()), Qt::UniqueConnection);
            connect(mpGraphicsView->mpVerticalFlipAction, SIGNAL(triggered()), SLOT(flipVertical()), Qt::UniqueConnection);
            connect(mpGraphicsView->mpRotateIconAction, SIGNAL(triggered()), SLOT(rotateClockwise()), Qt::UniqueConnection);
            connect(mpGraphicsView->mpRotateAntiIconAction, SIGNAL(triggered()), SLOT(rotateAntiClockwise()), Qt::UniqueConnection);
            connect(mpGraphicsView->mpResetRotation, SIGNAL(triggered()), SLOT(resetRotation()), Qt::UniqueConnection);
            connect(mpGraphicsView->mpDeleteIconAction, SIGNAL(triggered()), SLOT(deleteMe()), Qt::UniqueConnection);
            connect(mpGraphicsView, SIGNAL(keyPressDelete()), SLOT(deleteMe()), Qt::UniqueConnection);
            connect(mpGraphicsView, SIGNAL(keyPressUp()), SLOT(moveUp()), Qt::UniqueConnection);
            connect(mpGraphicsView, SIGNAL(keyPressDown()), SLOT(moveDown()), Qt::UniqueConnection);
            connect(mpGraphicsView, SIGNAL(keyPressLeft()), SLOT(moveLeft()), Qt::UniqueConnection);
            connect(mpGraphicsView, SIGNAL(keyPressRight()), SLOT(moveRight()), Qt::UniqueConnection);
            connect(mpGraphicsView, SIGNAL(keyPressRotateClockwise()), SLOT(rotateClockwise()), Qt::UniqueConnection);
            connect(mpGraphicsView, SIGNAL(keyPressRotateAntiClockwise()), SLOT(rotateAntiClockwise()), Qt::UniqueConnection);
        }
        // if use has clicked on corner item then dont make it passive
        else if (!mIsRectangleCorneItemClicked)
        {
            setSelectionBoxPassive();
            disconnect(mpGraphicsView->mpHorizontalFlipAction, SIGNAL(triggered()), this, SLOT(flipHorizontal()));
            disconnect(mpGraphicsView->mpVerticalFlipAction, SIGNAL(triggered()), this, SLOT(flipVertical()));
            disconnect(mpGraphicsView->mpRotateIconAction, SIGNAL(triggered()), this, SLOT(rotateClockwise()));
            disconnect(mpGraphicsView->mpRotateAntiIconAction, SIGNAL(triggered()), this, SLOT(rotateAntiClockwise()));
            disconnect(mpGraphicsView->mpResetRotation, SIGNAL(triggered()), this, SLOT(resetRotation()));
            disconnect(mpGraphicsView->mpDeleteIconAction, SIGNAL(triggered()), this, SLOT(deleteMe()));
            disconnect(mpGraphicsView, SIGNAL(keyPressDelete()), this, SLOT(deleteMe()));
            disconnect(mpGraphicsView, SIGNAL(keyPressUp()), this, SLOT(moveUp()));
            disconnect(mpGraphicsView, SIGNAL(keyPressDown()), this, SLOT(moveDown()));
            disconnect(mpGraphicsView, SIGNAL(keyPressLeft()), this, SLOT(moveLeft()));
            disconnect(mpGraphicsView, SIGNAL(keyPressRight()), this, SLOT(moveRight()));
            disconnect(mpGraphicsView, SIGNAL(keyPressRotateClockwise()), this, SLOT(rotateClockwise()));
            disconnect(mpGraphicsView, SIGNAL(keyPressRotateAntiClockwise()), this, SLOT(rotateAntiClockwise()));
        }
    }
    return value;
}

void ShapeAnnotation::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (!mIsCustomShape)
    {
        QGraphicsItem::contextMenuEvent(event);
        return;
    }
//    if (!mpGraphicsView)
//    {
//        QGraphicsItem::contextMenuEvent(event);
//        return;
//    }

  setSelected(true);
    QMenu menu(mpGraphicsView->mpParentProjectTab->mpParentProjectTabWidget->mpParentMainWindow);
    QMenu menuArrow(mpGraphicsView->mpParentProjectTab->mpParentProjectTabWidget->mpParentMainWindow);

    menu.addAction(mpGraphicsView->mpRotateIconAction);
   menu.addAction(mpGraphicsView->mpRotateAntiIconAction);
    menu.addAction(mpGraphicsView->mpResetRotation);
    //menu.addAction(mpGraphicsView->mpHorizontalFlipAction);
   // menu.addAction(mpGraphicsView->mpVerticalFlipAction);
    menu.addSeparator();
    menu.addAction(mpGraphicsView->mpDeleteIconAction);
    menu.addAction(mpShapePropertiesAction);
    if (qobject_cast<TextAnnotation*>(this))
      {
          menu.addAction(mpTextPropertiesAction);

      }

    if (qobject_cast<LineAnnotation*>(this))
      {
          menuArrow.setTitle("LineArrows");
          menuArrow.addAction(mpNoArrowAction);
          menuArrow.addAction(mpHalfArrowAction);
          menuArrow.addAction(mpOpenArrowAction);
         menuArrow.addAction(mpFilledArrowAction);
         menuArrow.addSeparator();
         menuArrow.addAction(mpNoEndArrowAction);
         menuArrow.addAction(mpHalfEndArrowAction);
         menuArrow.addAction(mpOpenEndArrowAction);
        menuArrow.addAction(mpFilledEndArrowAction);
          menu.addAction(menuArrow.menuAction());


      }

    //menu.addAction(mpShapePenStyleAction);
    //menu.addAction(mpShapeBrushStyleAction);
    menu.addSeparator();
    menu.exec(event->screenPos());
}

ShapeProperties::ShapeProperties(ShapeAnnotation *pShape, MainWindow *pParent)
    : QDialog(pParent, Qt::WindowTitleHint)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(400, 300);
    setModal(true);

    mpParentMainWindow = pParent;
    mpShape = pShape;
    // set the shape type, whether it is Line, Rectangle, Ellipse etc...
    setShapeType();
    // set up the dialog based on shape type.
    setUpDialog();
}

void ShapeProperties::setShapeType()
{
    //if the selected shape is a line
    if (qobject_cast<LineAnnotation*>(mpShape))
    {
        mShapeType = ShapeProperties::Line;
    }
    //if the selected shape is a text
    else if (qobject_cast<TextAnnotation*>(mpShape))
    {
        mShapeType = ShapeProperties::Text;
    }
    else
    {
        mShapeType= ShapeProperties::Rectangle;
    }
}

void ShapeProperties::setUpDialog()
{
    switch (mShapeType)
    {
    //only open pen properties if its a line
    case ShapeProperties::Line:
        setUpLineDialog();

        break;

    default:
        setUpLineDialog();

        break;
    }
}

void ShapeProperties::setUpLineDialog()
{
    setWindowTitle(QString(Helper::applicationName).append(" - Shape Properties"));

    mpHeadingLabel = new QLabel(tr("Properties"));
    mpHeadingLabel->setFont(QFont("", Helper::headingFontSize));
    mpHeadingLabel->setAlignment(Qt::AlignTop);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mpHeadingLabel);
    layout->addLayout(createHorizontalLine());
    layout->addLayout(createPenControls());
    if(mShapeType!=ShapeProperties::Line)
    {
    layout->addLayout(createBrushControls());
    }
    mpOkButton = new QPushButton(tr("OK"));
    mpOkButton->setAutoDefault(true);
    connect(mpOkButton, SIGNAL(clicked()), SLOT(applyChanges()));
    mpCancelButton = new QPushButton(tr("Cancel"));
    mpCancelButton->setAutoDefault(false);
    connect(mpCancelButton, SIGNAL(clicked()), SLOT(reject()));

    mpButtonBox = new QDialogButtonBox(Qt::Horizontal);
    mpButtonBox->addButton(mpOkButton, QDialogButtonBox::ActionRole);
    mpButtonBox->addButton(mpCancelButton, QDialogButtonBox::ActionRole);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(layout, 0, 0);
    mainLayout->addWidget(mpButtonBox, 4, 0);

    setLayout(mainLayout);
}

QVBoxLayout* ShapeProperties::createHorizontalLine()
{
    mpHorizontalLine = new QFrame();
    mpHorizontalLine->setFrameShape(QFrame::HLine);
    mpHorizontalLine->setFrameShadow(QFrame::Sunken);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mpHorizontalLine);
    return layout;
}

QVBoxLayout* ShapeProperties::createPenControls()
{
    mpPenStyleGroup = new QGroupBox(tr("Pen Style"));

    mpPenColorLabel = new QLabel(tr("Color:"));
    mpPenColorViewerLabel = new QLabel(tr(""));
    mpPenColorPickButton = new QPushButton(tr("Pick Color"));
    connect(mpPenColorPickButton, SIGNAL(clicked()), SLOT(pickPenColor()));
    mpPenNoColorCheckBox = new QCheckBox(tr("No Color"));
    connect(mpPenNoColorCheckBox, SIGNAL(stateChanged(int)), SLOT(penNoColorChecked(int)));

    mpPenPatternLabel = new QLabel(tr("Pattern:"));
    mpPenPatternsComboBox = new QComboBox;
    mpPenPatternsComboBox->setIconSize(Helper::iconSize);
    mpPenPatternsComboBox->addItem(QIcon(":/Resources/icons/solidline.png"), tr("Solid"), Qt::SolidLine);
    mpPenPatternsComboBox->addItem(QIcon(":/Resources/icons/dashline.png"), tr("Dash"), Qt::DashLine);
    mpPenPatternsComboBox->addItem(QIcon(":/Resources/icons/dotline.png"), tr("Dot"), Qt::DotLine);
    mpPenPatternsComboBox->addItem(QIcon(":/Resources/icons/dashdotline.png"), tr("Dash Dot"), Qt::DashDotLine);
    mpPenPatternsComboBox->addItem(QIcon(":/Resources/icons/dashdotdotline.png"), tr("Dash Dot Dot"), Qt::DashDotDotLine);

    mpPenThicknessLabel = new QLabel(tr("Thickness:"));
    mpPenThicknessSpinBox = new QDoubleSpinBox;
    // change the locale to C so that decimal char is changed from ',' to '.'
    mpPenThicknessSpinBox->setLocale(QLocale("C"));
    mpPenThicknessSpinBox->setRange(0.25, 100.0);
    mpPenThicknessSpinBox->setSingleStep(0.5);
    mpCornerRadiusLabel = new QLabel(tr("CornerRadius:"));
    mpCornerRadiusSpinBox = new QDoubleSpinBox;
    // change the locale to C so that decimal char is changed from ',' to '.'
    mpCornerRadiusSpinBox->setLocale(QLocale("C"));
    mpCornerRadiusSpinBox->setRange(0.0, 100.0);
    mpCornerRadiusSpinBox->setSingleStep(0.25);
    mpSmoothLabel = new QLabel(tr("Smooth:"));
    mpSmoothCheckBox = new QCheckBox(tr("Bezier Curve"));


    setInitPenColor(mpShape->getLineColor());
    QPixmap pixmap(Helper::iconSize);
    pixmap.fill(mPenColor);
    mpPenColorViewerLabel->setPixmap(pixmap);
    setInitPenPattern(mpShape->getLinePattern());
    setInitPenThickness(mpShape->getLineThickness());
    setInitCornerRadius(mpShape->getRectCornerRadius());
    setInitPenSmooth(mpShape->getLineSmooth());

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    mainLayout->addWidget(mpPenColorLabel, 0, 0);
    mainLayout->addWidget(mpPenColorViewerLabel, 0, 1);
    mainLayout->addWidget(mpPenColorPickButton, 1, 0);
    mainLayout->addWidget(mpPenNoColorCheckBox, 1, 1);
    if (mShapeType != Text)
    {
        mainLayout->addWidget(mpPenPatternLabel, 2, 0, 1, 2);
        mainLayout->addWidget(mpPenPatternsComboBox, 3, 0);
        mainLayout->addWidget(mpPenThicknessLabel, 4, 0, 1, 2);
        mainLayout->addWidget(mpPenThicknessSpinBox, 5, 0);
    }
    if (qobject_cast<RectangleAnnotation*>(mpShape))
    {
        mainLayout->addWidget(mpCornerRadiusLabel, 6, 0, 1, 2);
        mainLayout->addWidget(mpCornerRadiusSpinBox, 7, 0);
    }
    if (mShapeType == Line)
    {
        mainLayout->addWidget(mpSmoothLabel, 6, 0, 1, 2);
        mainLayout->addWidget(mpSmoothCheckBox, 7, 0);
    }
    mpPenStyleGroup->setLayout(mainLayout);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mpPenStyleGroup);
    return layout;
}

QVBoxLayout* ShapeProperties::createBrushControls()
{
    mpBrushStyleGroup = new QGroupBox(tr("Brush Style"));

    mpBrushColorLabel = new QLabel(tr("Color:"));
    mpBrushColorViewerLabel = new QLabel(tr(""));
    mpBrushColorPickButton = new QPushButton(tr("Pick Color"));
    connect(mpBrushColorPickButton, SIGNAL(clicked()), SLOT(pickBrushColor()));
    mpBrushNoColorCheckBox = new QCheckBox(tr("No Color"));
    connect(mpBrushNoColorCheckBox, SIGNAL(stateChanged(int)), SLOT(brushNoColorChecked(int)));

    mpBrushPatternLabel = new QLabel(tr("Pattern:"));
    mpBrushPatternsComboBox = new QComboBox;
    mpBrushPatternsComboBox->setIconSize(Helper::iconSize);
    mpBrushPatternsComboBox->addItem(tr("NoBrush"), Qt::NoBrush);
    mpBrushPatternsComboBox->addItem(QIcon(":/Resources/icons/solid.png"), tr("Solid"), Qt::SolidPattern);
    mpBrushPatternsComboBox->addItem(QIcon(":/Resources/icons/horizontal.png"), tr("Horizontal"), Qt::HorPattern);
    mpBrushPatternsComboBox->addItem(QIcon(":/Resources/icons/vertical.png"), tr("Vertical"), Qt::VerPattern);
    mpBrushPatternsComboBox->addItem(QIcon(":/Resources/icons/cross.png"), tr("Cross"), Qt::CrossPattern);
    mpBrushPatternsComboBox->addItem(QIcon(":/Resources/icons/forward.png"), tr("Forward"), Qt::BDiagPattern);
    mpBrushPatternsComboBox->addItem(QIcon(":/Resources/icons/backward.png"), tr("Backward"), Qt::FDiagPattern);
    mpBrushPatternsComboBox->addItem(QIcon(":/Resources/icons/crossdiag.png"), tr("CrossDiag"), Qt::DiagCrossPattern);
    mpBrushPatternsComboBox->addItem(QIcon(":/Resources/icons/horizontalcylinder.png"), tr("HorizontalCylinder"), Qt::LinearGradientPattern);
    mpBrushPatternsComboBox->addItem(QIcon(":/Resources/icons/verticalcylinder.png"), tr("VertitalCylinder"), Qt::Dense1Pattern);
    mpBrushPatternsComboBox->addItem(QIcon(":/Resources/icons/sphere.png"), tr("Sphere"), Qt::RadialGradientPattern);

    setInitBrushColor(mpShape->getFillColor());
    QPixmap pixmap(Helper::iconSize);
    pixmap.fill(mBrushColor);
    mpBrushColorViewerLabel->setPixmap(pixmap);
    setInitBrushPattern(mpShape->getFillPattern());

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    mainLayout->addWidget(mpBrushColorLabel, 0, 0);
    mainLayout->addWidget(mpBrushColorViewerLabel, 0, 1);
    mainLayout->addWidget(mpBrushColorPickButton, 1, 0);
    mainLayout->addWidget(mpBrushNoColorCheckBox, 1, 1);
    if (mShapeType != Text)
    {
        mainLayout->addWidget(mpBrushPatternLabel, 2, 0, 1, 2);
        mainLayout->addWidget(mpBrushPatternsComboBox, 3, 0);
    }
    mpBrushStyleGroup->setLayout(mainLayout);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mpBrushStyleGroup);
    return layout;
}

//functions to set initial settings in the properties widget
void ShapeProperties::setInitPenPattern(Qt::PenStyle pattern)
{
    int index = mpPenPatternsComboBox->findData(pattern);
    if (index != -1)
        mpPenPatternsComboBox->setCurrentIndex(index);
}

void ShapeProperties::setInitCornerRadius(double radius)
{
    mpCornerRadiusSpinBox->setValue(radius);
}

void ShapeProperties::setInitPenThickness(double thickness)
{
    mpPenThicknessSpinBox->setValue(thickness);
}

void ShapeProperties::setInitPenSmooth(bool smooth)
{
    mpSmoothCheckBox->setChecked(smooth);
}

void ShapeProperties::pickPenColor()
{
    QColor color = QColorDialog::getColor();

    if (color.spec() == QColor::Invalid)
        return;

    mPenColor = color;
    if (mpPenNoColorCheckBox->checkState() == Qt::Unchecked)
    {
        QPixmap pixmap(Helper::iconSize);
        pixmap.fill(mPenColor);
        mpPenColorViewerLabel->setPixmap(pixmap);
    }
}

void ShapeProperties::setInitPenColor(QColor color)
{
    mPenColor = color;
}

QColor ShapeProperties::getPenColor()
{
    // if user selects no pen color and selects a brush color then return pen color as transparent
    if ((mpPenNoColorCheckBox->checkState() == Qt::Checked) and
        (mBrushColor.spec() != QColor::Invalid))
        return Qt::transparent;
    // if user selects no pen color and selects no brush color then return pen color as black(default)
    else if ((mpPenNoColorCheckBox->checkState() == Qt::Checked) and
             (mBrushColor.spec() == QColor::Invalid))
        return Qt::black;
    else
        return mPenColor;
}

void ShapeProperties::setPenPattern()
{
    //int index = mpPenPatternsComboBox->findText(pattern, Qt::MatchExactly);
    //if (index != -1)
    //mpPenPatternsComboBox->setCurrentIndex(index);
    if(mpPenPatternsComboBox->currentIndex()== -1)
    {
     mPenPattern= Qt::SolidLine;
    }
    else
    {
    mPenPattern = Qt::PenStyle(mpPenPatternsComboBox->itemData(mpPenPatternsComboBox->currentIndex()).toInt());
    }
}

QString ShapeProperties::getPenPatternString()
{
    return mpPenPatternsComboBox->currentText();
}

Qt::PenStyle ShapeProperties::getPenPattern()
{
    //return Qt::PenStyle(mpPenPatternsComboBox->itemData(mpPenPatternsComboBox->currentIndex()).toInt());
    return mPenPattern;
}


void ShapeProperties::penNoColorChecked(int state)
{
    if (state == Qt::Checked)
    {
        if (mBrushColor.spec() == QColor::Invalid)
        {
            QPixmap pixmap(Helper::iconSize);
            pixmap.fill(Qt::black);
            mpPenColorViewerLabel->setPixmap(pixmap);
        }
        else
        {
            QPixmap pixmap(Helper::iconSize);
            pixmap.fill(Qt::transparent);
            mpPenColorViewerLabel->setPixmap(pixmap);
        }
    }
    else if (state == Qt::Unchecked)
    {
        if (mPenColor.spec() != QColor::Invalid)
        {
            QPixmap pixmap(Helper::iconSize);
            pixmap.fill(mPenColor);
            mpPenColorViewerLabel->setPixmap(pixmap);
        }
    }
}

void ShapeProperties::setCornerRadius()
{
    mRectCornerRadius = mpCornerRadiusSpinBox->value();
}

double ShapeProperties::getCornerRadius()
{
    return mRectCornerRadius;
}

void ShapeProperties::setPenThickness()
{
    mPenThickness = mpPenThicknessSpinBox->value();
}

double ShapeProperties::getPenThickness()
{
    return mPenThickness;
}


void ShapeProperties::setPenSmooth()
{
   mPenSmooth= mpSmoothCheckBox->isChecked();
}

bool ShapeProperties::getPenSmooth()
{
    return mPenSmooth;
}

void ShapeProperties::setInitBrushColor(QColor color)
{
    mBrushColor = color;
}

void ShapeProperties::setInitBrushPattern(Qt::BrushStyle pattern)
{
    int index = mpBrushPatternsComboBox->findData(pattern);
    if (index != -1)
        mpBrushPatternsComboBox->setCurrentIndex(index);
}

void ShapeProperties::pickBrushColor()
{
    QColor color = QColorDialog::getColor();

    if (color.spec() == QColor::Invalid)
        return;

    mBrushColor = color;



    if (mpBrushNoColorCheckBox->checkState() == Qt::Unchecked)
    {
        QPixmap pixmap(Helper::iconSize);
        pixmap.fill(mBrushColor);
        mpBrushColorViewerLabel->setPixmap(pixmap);
    }
}


QColor ShapeProperties::getBrushColor()
{
    if (mpBrushNoColorCheckBox->checkState() == Qt::Checked)
        return Qt::transparent;
    else
        return mBrushColor;
}

void ShapeProperties::setBrushPattern()
{
    //int index = mpPenPatternsComboBox->findText(pattern, Qt::MatchExactly);
    //if (index != -1)
    //mpPenPatternsComboBox->setCurrentIndex(index);
    if(mpBrushPatternsComboBox->currentIndex()== -1)
    {
     mBrushPattern= Qt::NoBrush;
    }
    else
    {
    mBrushPattern = Qt::BrushStyle(mpBrushPatternsComboBox->itemData(mpBrushPatternsComboBox->currentIndex()).toInt());
    }
}

QString ShapeProperties::getBrushPatternString()
{
    return mpBrushPatternsComboBox->currentText();
}

Qt::BrushStyle ShapeProperties::getBrushPattern()
{
    //return Qt::PenStyle(mpPenPatternsComboBox->itemData(mpPenPatternsComboBox->currentIndex()).toInt());
    return mBrushPattern;
}

void ShapeProperties::brushNoColorChecked(int state)
{
    if (state == Qt::Checked)
    {
        QPixmap pixmap(Helper::iconSize);
        pixmap.fill(Qt::transparent);
        mpBrushColorViewerLabel->setPixmap(pixmap);
    }
    else if (state == Qt::Unchecked)
    {
        QPixmap pixmap(Helper::iconSize);
        pixmap.fill(mBrushColor);
        mpBrushColorViewerLabel->setPixmap(pixmap);
    }
}

void ShapeProperties::applyChanges()
{
    setPenPattern();
    setPenThickness();
    setCornerRadius();
    setPenSmooth();
    mpShape->changePenProperty();
    if(mShapeType!=ShapeProperties::Line)
    {
        setBrushPattern();
        mpShape->changeBrushProperty();
    }
    accept();
}

