"use strict";

// define infovis package / conglomerate- / accumulation-object
if(window.infovis == undefined){
	window.infovis = {};
}

// class SVGCanvas
// uses D3

// attributes
// - _svgSelection: d3 selection of SVG node
// - _width: target width of SVG node
// - _height: taret height of SVG node

// constructor
// @param parentNode - node to be used as parent for created SVG node
// @param layoutParameters -
// 	@param width - target width of SVG node
// 	@param height - target height of SVG node
infovis.SVGCanvas = function(parentNode, layoutParameters){

	// initialize attributes
	this._svgSelection = d3.select(parentNode).append("svg");

	// initialize indirect attributes
	this._svgSelection
		.attr("xmlns", "http://www.w3.org/2000/svg")
		.attr("version", "1.1");

	this.setLayoutParameters(layoutParameters);

	return this;
}

// methods
// @param layoutParameters -
// 	@param width - target width of SVG node
// 	@param height - target height of SVG node
infovis.SVGCanvas.prototype.setLayoutParameters = function(layoutParameters){

	var standardLayoutParameters = {
		width: 600,
		height: 400
	}

	this._width = infovis.firstExisting([layoutParameters, "width"], [this, "_width"], standardLayoutParameters.width);
	this._height = infovis.firstExisting([layoutParameters, "height"], [this, "_height"], standardLayoutParameters.height);

	this._svgSelection
		.attr("width", this._width)
		.attr("height", this._height);
}

// returns property of first object where given property name is in object or default
// @param [[object0, propertyName0], ... [objectN, propertyNameN], default]
infovis.firstExisting = function(){

	var result = arguments[arguments.length-1];
	var foundResult = false;

	for(var i = 0; !foundResult && i < arguments.length-1; i++){

		var propertyName = arguments[i][1];

		var object = arguments[i][0];


		if(typeof(object) == "object" && propertyName in object){
			result = object[propertyName];
			foundResult = true;
		}
	}

	return result;
}
