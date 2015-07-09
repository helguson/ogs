
// define infovis package / conglomerate- / accumulation-object
if(window.infovis == undefined){
	window.infovis = {};
}

(function(container){


/**
 * - _svgNode
 * - _group
 * - _backgroundRect
 * - _text

 * - _backgroundPadding
 * - _textXOffset
 * - _textYOffset
 * - _lineYOffset
 * - _alignRelativeToGivenPosition
 */
container.SVGHoverInfo = function(svgNode, layoutParameter){


	this._setUpDOMElements(svgNode);

	this.setLayoutParameters(layoutParameter);

	this.setVisibilityTo(false);

	return this;
}

container.SVGHoverInfo.prototype._setUpDOMElements = function(svgNode){

	this._svgNode = svgNode;

	this._group = d3.select(this._svgNode).append("g")
		.attr("class", "hover");


	this._backgroundRect = this._group.append("rect")
		.attr(
			"class",
			"background"
		);

	this._text = this._group.append("text");
}

container.SVGHoverInfo.prototype.setLayoutParameters = function(layoutParameters){

	//#######################
	//### private methods ###
	//#######################
	// returns property of first object where given property name is in object or default
	// @param [[object0, propertyName0], ... [objectN, propertyNameN], default]
	var firstExisting = function(){

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


	//#################
	//### constants ###
	//#################
	var standardLayoutParameters = {
		backgroundPadding: 5,
		textXOffset: "1.5em",
		textYOffset: "1.1em",
		lineYOffset: "1.1em",
		alignRelativeToGivenPosition: "left"
	}

	//############
	//### main ###
	//############
	this._backgroundPadding = firstExisting(
		[layoutParameters, "backgroundPadding"],
		[this, "_backgroundPadding"],
		standardLayoutParameters["backgroundPadding"]
	);
	this._textXOffset = firstExisting(
		[layoutParameters, "textXOffset"],
		[this, "_textXOffset"],
		standardLayoutParameters["textXOffset"]
	);
	this._textYOffset = firstExisting(
		[layoutParameters, "textYOffset"],
		[this, "_textYOffset"],
		standardLayoutParameters["textYOffset"]
	);
	this._lineYOffset = firstExisting(
		[layoutParameters, "lineYOffset"],
		[this, "_lineYOffset"],
		standardLayoutParameters["lineYOffset"]
	);
	this._alignRelativeToGivenPosition = firstExisting(
		[layoutParameters, "alignRelativeToGivenPosition"],
		[this, "_alignRelativeToGivenPosition"],
		standardLayoutParameters["alignRelativeToGivenPosition"]
	);

	this._updateTextProperties();
	this._updateBackground();
}

// relative to svg node
container.SVGHoverInfo.prototype.setPosition = function(x, y){

	// TODO
	this._group
		.attr(
			"transform",
			"translate(" + x + ", " + y + ")"
		);

	// update text and background
}

container.SVGHoverInfo.prototype.setVisibilityTo = function(v){

	this._group
		.style(
			"visibility",
			(v == true)? null: "hidden"	//null -> remove visibility style; "hidden" -> "visibility: hidden"
		);
}

container.SVGHoverInfo.prototype.setTextTo = function(text){

	var self = this;
	var lineData = text.split("\n");

	var lineSelection = this._text.selectAll("tspan").data(lineData);

	lineSelection.exit()
		.remove();

	lineSelection.enter()
		.append("tspan")
			.attr(
				"x",
				this._alignRelativeToGivenPosition == "left"? this._textXOffset : "-"+this._textXOffset
			);

	lineSelection
		.text(
			function(d){ return d; }
		)
		.attr(
			"dy",
			function(datum, index){
				return index==0? self._textYOffset: self._lineYOffset;
			}
		);

	this._updateBackground();

}


container.SVGHoverInfo.prototype._updateBackground = function(){

	var textDimensions = this._text.node().getBBox();

	this._backgroundRect
		.attr(
			"x",
			textDimensions.x - this._backgroundPadding
		)
		.attr(
			"y",
			textDimensions.y - this._backgroundPadding
		)
		.attr(
			"width",
			textDimensions.width + 2*this._backgroundPadding
		)
		.attr(
			"height",
			textDimensions.height + 2*this._backgroundPadding
		);
}

container.SVGHoverInfo.prototype._updateTextProperties = function(){

	this._text
		.attr(
			"text-anchor",
			this._alignRelativeToGivenPosition == "left"? "start" : "end"
		);

	this._text.selectAll("tspan")
		.attr(
			"x",
			this._alignRelativeToGivenPosition == "left"? this._textXOffset : "-"+this._textXOffset
		)
}


})(infovis);
