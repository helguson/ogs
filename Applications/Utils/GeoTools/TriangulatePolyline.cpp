/**
 * \file   TriangulatePolyline.h
 * \author Karsten Rink
 * \date   2015-02-02
 * \brief  Utility for triangulating polylines.
 *
 * Copyright (c) 2012-2015, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/LICENSE.txt
 *
 */

#include <string>

#include "logog/include/logog.hpp"
#include "tclap/CmdLine.h"

#include "BaseLib/BuildInfo.h"
#include "BaseLib/LogogSimpleFormatter.h"
#include "FileIO/XmlIO/Qt/XmlGmlInterface.h"
#include "GeoLib/AnalyticalGeometry.h"
#include "GeoLib/GEOObjects.h"
#include "GeoLib/Polyline.h"

#include <QApplication>


std::string output_question()
{
	WARN ("Given polyline is not closed. Close polyline now?");
	WARN ("Enter (Y)es for closing the line or (N)o for abort and press ENTER.");
	std::string input;
	std::cin >> input;
	return input;
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv, false);

	LOGOG_INITIALIZE();
	BaseLib::LogogSimpleFormatter *custom_format (new BaseLib::LogogSimpleFormatter);
	logog::Cout *logogCout(new logog::Cout);
	logogCout->SetFormatter(*custom_format);

	TCLAP::CmdLine cmd("Triangulates the specified polyline in the given geometry file.", ' ', BaseLib::BuildInfo::git_describe);
	TCLAP::ValueArg<std::string>  input_arg("i", "input",  "GML input file (*.gml)", true, "", "string");
	TCLAP::ValueArg<std::string> output_arg("o", "output", "GML output file (*.gml)", true, "", "string");
	TCLAP::ValueArg<std::string>   name_arg("n", "name",   "Name of polyline in given file", true, "", "string");
	cmd.add( input_arg );
	cmd.add( name_arg );
	cmd.add( output_arg );
	cmd.parse( argc, argv );

	std::string const& file_name(input_arg.getValue());
	std::string const& polyline_name(name_arg.getValue());

	GeoLib::GEOObjects geo_objects;
	FileIO::XmlGmlInterface xml(geo_objects);
	if (!xml.readFile(file_name))
	{
		ERR ("Failed to load geometry file.");
		return 1;
	}

	std::vector<std::string> geo_names;
	geo_objects.getGeometryNames(geo_names);
	GeoLib::PolylineVec const*const line_vec (geo_objects.getPolylineVecObj(geo_names[0]));
	GeoLib::Polyline* line = const_cast<GeoLib::Polyline*>(line_vec->getElementByName(polyline_name));

	// check if line exists
	if (line == nullptr)
	{
		ERR ("No polyline found with name \"%s\". Aborting...", polyline_name.c_str());
		return 1;
	}

	// check if polyline can be triangulated (i.e. closed + coplanar)
	if (!line->isCoplanar())
	{
		ERR ("Polyline is not coplanar, no unambiguous triangulation possible. Aborting...");
		return 1;
	}

	if (!line->isClosed())
	{
		std::string input ("");
		while (input != "y" && input != "Y" && input != "n" && input != "N")
			input = output_question();
		
		if (input == "y" || input == "Y")
		{
			line->closePolyline();
			INFO ("Polyline closed.");
		}
		else
			return 1;
	}

	// create surface
	INFO ("Triangulating surface...");
	std::vector<GeoLib::Surface*> *new_sfc = new std::vector<GeoLib::Surface*>;
	new_sfc->push_back(GeoLib::Surface::createSurface(*line));

	GeoLib::SurfaceVec* sfc_vec (geo_objects.getSurfaceVecObj(geo_names[0]));
	if (sfc_vec == nullptr)
	{
		geo_objects.addSurfaceVec(new_sfc, geo_names[0]);
		sfc_vec = geo_objects.getSurfaceVecObj(geo_names[0]);
	}
	else
		geo_objects.appendSurfaceVec(*new_sfc, geo_names[0]);
	std::size_t const sfc_id = geo_objects.getSurfaceVec(geo_names[0])->size() - 1;
	std::string const surface_name (polyline_name + "_surface");
	for (std::size_t i=1;;++i)
	{
		std::string const new_surface_name = (i>1) ? (surface_name + std::to_string(i)) : surface_name;
		if (sfc_vec->getElementByName(new_surface_name) == nullptr)
		{
			sfc_vec->setNameForElement(sfc_id, new_surface_name);
			break;
		}
	}

	// write new file
	xml.setNameForExport(geo_names[0]);
	xml.writeToFile(output_arg.getValue());
	INFO ("...done.");

	delete logogCout;
	delete custom_format;
	LOGOG_SHUTDOWN();
	return 0;
}
