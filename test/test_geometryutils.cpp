/***************************************************************************
                        test_geometryutils.h
                        --------------------
  begin                : Jun 2020
  copyright            : (C) 2020 by Ivan Ivanov
  email                : ivan@opengis.ch
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#define CATCH_CONFIG_MAIN

#include "rubberbandmodel.h"
#include "utils/geometryutils.h"

#include <qgis.h>
#include <qgsvectorlayer.h>

#include "catch2.h"


TEST_CASE( "GeometryUtils" )
{
  std::unique_ptr<RubberbandModel> model = std::make_unique<RubberbandModel>();
  std::unique_ptr<QgsVectorLayer> mLayer = std::make_unique<QgsVectorLayer>( QStringLiteral( "Polygon?crs=epsg:3946" ), QStringLiteral( "vl" ), QStringLiteral( "memory" ) );

  QgsFeature f( mLayer->fields(), 1 );
  f.setGeometry( QgsGeometry::fromWkt( QStringLiteral( "Polygon ((8 8, 9 8, 8 9, 8 8))" ) ) );

  REQUIRE( mLayer->startEditing() );
  REQUIRE( mLayer->addFeature( f ) );
  REQUIRE( mLayer->commitChanges() );

  model->setGeometryType( mLayer->geometryType() );

  SECTION( "PolygonFromRubberband" )
  {
    const QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem::fromEpsgId( 3946 );

    model->addVertexFromPoint( QgsPoint( 0, 0 ) );
    model->addVertexFromPoint( QgsPoint( 2, 1 ) );
    model->addVertexFromPoint( QgsPoint( 1, 2 ) );

    QgsGeometry geom = GeometryUtils::polygonFromRubberband( model.get(), crs );

    REQUIRE( geom.asWkt() == QStringLiteral( "Polygon ((0 0, 2 1, 1 2, 1 2, 0 0))" ) );
  }

  SECTION( "   AddRingFromRubberband" )
  {
    REQUIRE( mLayer->startEditing() );
#if _QGIS_VERSION_INT >= 32100
    REQUIRE( GeometryUtils::addRingFromRubberband( mLayer.get(), 100, model.get() ) == Qgis::GeometryOperationResult::AddRingNotInExistingFeature );
#else
    REQUIRE( GeometryUtils::addRingFromRubberband( mLayer.get(), 100, model.get() ) == QgsGeometry::AddRingNotInExistingFeature );
#endif

    model->addVertexFromPoint( QgsPoint( 8.1, 8.1 ) );
    model->addVertexFromPoint( QgsPoint( 8.9, 8.1 ) );
    model->addVertexFromPoint( QgsPoint( 8.1, 8.9 ) );
    mLayer->select( 1 );

#if _QGIS_VERSION_INT >= 32100
    REQUIRE( GeometryUtils::addRingFromRubberband( mLayer.get(), 1, model.get() ) == Qgis::GeometryOperationResult::Success );
#else
    REQUIRE( GeometryUtils::addRingFromRubberband( mLayer.get(), 1, model.get() ) == QgsGeometry::Success );
#endif
    REQUIRE( mLayer->rollBack() );
  }


  SECTION( "SplitFeatureFromRubberband" )
  {
    REQUIRE( mLayer->startEditing() );
#if _QGIS_VERSION_INT >= 32100
    REQUIRE( GeometryUtils::splitFeatureFromRubberband( mLayer.get(), model.get() ) == Qgis::GeometryOperationResult::NothingHappened );
#else
    REQUIRE( GeometryUtils::splitFeatureFromRubberband( mLayer.get(), model.get() ) == QgsGeometry::NothingHappened );
#endif

    model->addVertexFromPoint( QgsPoint( 7.5, 8.5 ) );
    model->addVertexFromPoint( QgsPoint( 9.5, 8.5 ) );
    mLayer->select( 1 );

#if _QGIS_VERSION_INT >= 32100
    REQUIRE( GeometryUtils::splitFeatureFromRubberband( mLayer.get(), model.get() ) == Qgis::GeometryOperationResult::Success );
#else
    REQUIRE( GeometryUtils::splitFeatureFromRubberband( mLayer.get(), model.get() ) == QgsGeometry::Success );
#endif
    REQUIRE( mLayer->rollBack() );
  }

}
