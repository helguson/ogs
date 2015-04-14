"use strict";

// define infovis package / conglomerate- / accumulation-object
if(window.infovis == undefined){
	window.infovis = {};
}

(function(container){

//#######################
//### HistogramCanvas ###
//#######################
/**
 * uses d3.js, inheritance.js
 * inherits SVGCanvas from SVGCanvas.js
 *
 * attributes
 * - _baseDataAccessor - accessor to bins
 * - _degreeOfInterestList - degree of interest for each DataAccessor in baseDataAccessor
 * - _maximumInterestedValueToMaximumShownValueFactor -
 * - _xAxisHeight -
 * - _yAxisWidth -
 * - _barsGroup - D3 selection of <g> that contains the bars
 * - _xAxisGroup - D3 selection of <g> that contains elements that form the x axis
 * - _yAxisGroup - D3 selection of <g> that contains elements that form the y axis
 */

/**
 * @param layoutParameters -
 * 	@property width - target width of SVG node
 * 	@property height - target height of SVG node
 * 	@property xAxisHeight - target height of x axis
 * 	@property yAxisWidth - target width of y axis
 * @param dataParameters
 * 	@property baseDataAccessor - accessor to bins
 * 	@property degreeOfInterestList - degree of interest for each DataAccessor in baseDataAccessor
 */
container.HistogramCanvas = function(parentNode, layoutParameters, dataParameters){

	var instance = container.HistogramCanvas.applyParentConstructorTo(
		this,
		[parentNode]
	);

	instance._setUpDOMStructure();

	instance.setDataParameters(dataParameters);
	instance.setLayoutParameters(layoutParameters);

	this.render();

	return instance;
}
container.HistogramCanvas.extend(infovis.SVGCanvas);

container.HistogramCanvas.prototype._setUpDOMStructure = function(){

	this._barsGroup = this._svgSelection.append("g")
		.attr("class", "bars");
	this._xAxisGroup = this._svgSelection.append("g")
		.attr("class", "x_axis");
	this._yAxisGroup = this._svgSelection.append("g")
		.attr("class", "y_axis");
}

// @param dataParameters
//	@property baseDataAccessor - accessor to bins
//	@property degreeOfInterestList - degree of interest for each DataAccessor in baseDataAccessor
container.HistogramCanvas.prototype.setDataParameters = function(dataParameters){

	this._baseDataAccessor = infovis.firstExisting([dataParameters, 'baseDataAccessor'], this._baseDataAccessor);
	this._degreeOfInterestList = infovis.firstExisting([dataParameters, 'degreeOfInterestList'], this._degreeOfInterestList);

	if(this._baseDataAccessor.length != this._degreeOfInterestList.length){
		throw new Error("number of degrees of interest do not equal number of bins");
	}
}

container.HistogramCanvas.prototype.setLayoutParameters = function(layoutParameters){

	// call parent setLayoutParameters
	var parentPrototype = container.HistogramCanvas.prototype.__proto__;
	parentPrototype.setLayoutParameters.call(this, layoutParameters);

	var standardLayoutParameters = {
		maximumInterestedValueToMaximumShownValueFactor: 1.1,
		xAxisHeight: 50,
		yAxisWidth: 50
	}

	this._maximumInterestedValueToMaximumShownValueFactor = infovis.firstExisting(
		[layoutParameters, "maximumInterestedValueToMaximumShownValueFactor"],
		[this, "_maximumInterestedValueToMaximumShownValueFactor"],
		standardLayoutParameters.maximumInterestedValueToMaximumShownValueFactor
	);
	this._xAxisHeight = infovis.firstExisting(
		[layoutParameters, "xAxisHeight"],
		[this, "_xAxisHeight"],
		standardLayoutParameters.xAxisHeight
	);
	this._yAxisWidth = infovis.firstExisting(
		[layoutParameters, "yAxisWidth"],
		[this, "_yAxisWidth"],
		standardLayoutParameters.yAxisWidth
	);
}

container.HistogramCanvas.prototype._prepareData = function(){
	//#######################
	//### private methods ###
	//#######################
	var pairs = function(a, b){
		if(a.length != b.length){
			throw new Error("cannot combine arrays of unequal length");
		}

		var result = [];

		for(var i = 0; i < a.length; i++){
			result.push(
				[a[i], b[i]]
			);
		}

		return result;
	}

	//############
	//### main ###
	//############

	return pairs(this._baseDataAccessor.split() ,this._degreeOfInterestList);
}

container.HistogramCanvas.prototype._createScales = function(data, maxShownValue){

	var xScale = d3.scale.ordinal()
		.domain(
			data.map(
				function(element, index){
					return index;
				}
			)
		)
		.rangeBands([0, this._width], 0, 0.05);


	var yScale = d3.scale.linear()
		.domain([0, maxShownValue])
		.range([0, this._height]);

	return {
		x: xScale,
		y: yScale
	}
}

container.HistogramCanvas.prototype._determineMaxCountOfInterest = function(data){

	var countsOfInterest = data
		.filter(
			function(e){
				var degreeOfInterest = e[1];
				var elementIsOfInterest = degreeOfInterest == 1;

				return elementIsOfInterest;
			}
		)
		.map(
			function(e){
				return e[0].access();
			}
		);

	return Math.max.apply(Math, countsOfInterest);
}

container.HistogramCanvas.prototype._determineMaxShownValue = function(maxCountOfInterest){

	return maxCountOfInterest * this._maximumInterestedValueToMaximumShownValueFactor;
}

container.HistogramCanvas.prototype._createBars = function(data, scales, maxShownValue){

	//#######################
	//### private methods ###
	//#######################
	// @param this - barSelection
	// @param canvasHeight - height of Area for bars
	var setGeometricalAttributes = function(scales, canvasHeight, rectangleWidth){

		this
			.attr(
				"x",
				function(datum, index){
					return scales.x(index);
				}
			)
			.attr(
				"y",
				function(datum, index){
					var count = datum[0].access();

					return canvasHeight - scales.y(count);
				}
			)
			.attr(
				"width",
				rectangleWidth
			)
			.attr(
				"height",
				function(datum, index){
					var count = datum[0].access();

					return scales.y(count);
				}
			);
	}

	// set class by DegreeofInterest
	// @param this - barSelection
	var setClassAttributes = function(maxShownValue){
		this
			.attr(
				"class",
				function(datum, index){
					var result;

					var datumAccessor = datum[0];
					var degreeOfInterest = datum[1];

					if(degreeOfInterest == 1){
						result = "barOfInterest";
					}
					else{
						var count = datumAccessor.access();

						if(count > maxShownValue){
							result = "partiallyShownContextBar";
						}
						else{
							result = "contextBar";
						}
					}

					return result;
				}
			);
	}

	//############
	//### main ###
	//############


	var barSelection = this._barsGroup.selectAll("rect");

	barSelection = barSelection.data(data);

	barSelection.exit()
		.remove();
	barSelection.enter()
		.append("rect");


	var rectangleWidth = scales.x.rangeBand();
	setGeometricalAttributes.call(barSelection, scales, this._height, rectangleWidth);

	setClassAttributes.call(barSelection, maxShownValue);
}

container.HistogramCanvas.prototype.render = function(){

	var data = this._prepareData();

	// determine some meta data
	var maxCountOfInterest = this._determineMaxCountOfInterest(data);
	var maxShownValue = this._determineMaxShownValue(maxCountOfInterest);

	var scales = this._createScales(data, maxShownValue);

	this._createBars(data, scales, maxShownValue);
}


})(infovis);
