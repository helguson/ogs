"use strict";

// define infovis package / conglomerate- / accumulation-object
window.infovis = {};

// removes error message of unavailability of JavaScript in case of availability of JavaScript
function removeMessageOfUnavailabilityOfJavaScript(){

	document.getElementById("javaScriptNotExecutedDummy").remove();
}

// TODO: think about the robustnes of this method
// TODO: test
// returns whether d3.js is available
function isD3Available(){

	return window["d3"] != undefined;
}

// TODO: make more common
// TODO: test
// assertion: qtPushButton should have been passed from Qt side before execution of this code
function isQtPresent(){

	return window["qtPushButton"] != undefined;
}

function setUpSampleSVG(){

	infovis.sampleSVG = d3.select("#viz")
		.append("svg")
		.attr("width", 100)
		.attr("height", 100);

	infovis.sampleSVG.append("circle")
		.style("stroke", "gray")
		.style("fill", "white")
		.attr("r", 40)
		.attr("cx", 50)
		.attr("cy", 50)
		.on("mouseover", function(){d3.select(this).style("fill", "aliceblue");})
		.on("mouseout", function(){d3.select(this).style("fill", "white");});
}

// TODO: test - Does this work? If yes, does it mean non linear execution?
function setUpEventPassingFromQtToJS(){

	// qtPushButton's signal 'clicked' is connected with a JavaScript function
	qtPushButton.clicked.connect(someFunction);
	var buttonText = qtPushButton.text;
	function someFunction()
	{
		d3.select("#viz")
			.append("text")
			.text("Clicked! ");

		d3.select("#viz").selectAll("circle").style("fill", "red");

		// Set back the buttons text (see below)
		qtPushButton.text = buttonText;
	}
}

function setUpEventPassingFromJSToQt(){

	// Pass data back to Qt
	infovis.sampleSVG.selectAll("circle").on(
		"click",
		function(){
			qtPushButton.text = "Javascript takes over!";
		}
	);
}

infovis.DataAccessorFactory = function(values, metaDataRelation){

	var DataAccessor = infovis.DataAccessor;

	this.values = values;
	this.metaDataRelation = metaDataRelation;

	this.makeAccessorFrom = function(indexList){

		return infovis.DataAccessor.inject(indexList, this);
	};
}

/**
 * a DataAccessor is an JavaScript array that stores indices (integers >= 0 or undefined) or other DataAccessors
 * DataAccessor is refered as 'accessor' in the following
**/
infovis.DataAccessor = {};
var DataAccessor = infovis.DataAccessor;

DataAccessor.inject = function(baseDataIndices, dataStorage){

	//#######################
	//### private methods ###
	//#######################
	/**
	* @brief applies 'injectPropertiesInto' to 'result'
	* and to every array that is stored within 'result'
	**/
	var injectProperties = function(base){

		var injectables = [base];

		while(injectables.length > 0){
			// dequeue first element
			var currentArray = injectables.shift();

			var additionalInjectables = currentArray.filter(Array.isArray);

			// enqueue additional elements
			injectables = injectables.concat(
				additionalInjectables
			);

			injectPropertiesInto(currentArray);
		}
	}

	var injectPropertiesInto = function(object){

		injectNonPrototypicPropertiesInto(object);
		injectPrototypicPropertiesInto(object, DataAccessor.prototype);
	};

	var injectNonPrototypicPropertiesInto = function(object){

		object.dataStorage = dataStorage;
	};

	var injectPrototypicPropertiesInto = function(object, prototype){

		for(var key in prototype){
			if(prototype.hasOwnProperty(key)){
				Object.defineProperty(
					object,
					key,
					{value: prototype[key]}
				);
			}
		}
	};

	//#############
	//### setUp ###
	//#############

	injectProperties(baseDataIndices);

	return baseDataIndices;
}

DataAccessor.prototype = {};

DataAccessor.prototype.access = function(value){
	//#######################
	//### private methods ###
	//#######################
	// @param this - current accessor
	var accessAll = function(){

		var thisAccessor = this;

		return this.map(
			function(element){
				// element is sub array
				if(Array.isArray(element)){
					return element.access();
				}
				// element is valueIndex
				else{
					return thisAccessor.dataStorage.values[element];
				}
			}
		)
	};

	// @param this - current accessor
	var accessElements = function(value){
		var result;

		if(typeof(value) == "function"){

			result = accessElementsByPredicateFunction.call(this, value);
		}
		else{

			result = accessElementsByMetaDate.call(this, value);
		}

		return result;
	};

	// @param this - current accessor
	var accessElementsByMetaDate = function(metaDate){

		return accessElementsByPredicateFunction.call(
			this,
			function(index, element){

				var metaData = element.meta()[0].access();

				// "return metaData.contains(metaDate);"
				return metaData.indexOf(metaDate) != -1;
			}
		);
	};

	/**
	 * @brief returns list of values which return true on application of predicate function
	 * @param this - current accessor
	 * @param predicateFunction(index, element)
	 * 	this - current accessor
	 * 	index - index of element within the current accessor
	 * 	element - accessor for the current element
	**/
	var accessElementsByPredicateFunction = function(predicateFunction){

		var result = [];

		for(var internalIndex = 0; internalIndex < this.length; internalIndex++){

			var internalElement = this[internalIndex];

			if(Array.isArray(internalElement)){

				var values = internalElement.access(predicateFunction);

				if(values.length > 0){
					result.push(values);
				}
			}
			else{

				var elementAccessor = DataAccessor.inject(
					[this[internalIndex]],
					this.dataStorage
				);

				var fulfiledPredicate = predicateFunction.call(
					this,
					internalIndex,
					elementAccessor
				);

				if(fulfiledPredicate){

					var value = elementAccessor.access()[0];
					result.push(value);
				}
			}

		}

		return result;
	};



	//############
	//### main ###
	//############
	var result;

	if(arguments.length == 0){
		result = accessAll.call(this);
	}
	else{
		result = accessElements.call(this, value);
	}

	return result;
};

DataAccessor.prototype.meta = function(value){
	//#######################
	//### private methods ###
	//#######################
	// @param this - current accessor
	var metaLists = function(){

		var thisAccessor = this;

		var listOfMetaAccessors = thisAccessor.map(
			function(element){

				if(Array.isArray(element)){

					return element.meta();
				}
				else{
					// create copy instead of modifying stored data
					var metaDataIndices = [].concat(
						thisAccessor.dataStorage.metaDataRelation[element]
					);

					return DataAccessor.inject(
						metaDataIndices,
						thisAccessor.dataStorage
					);
				}
			}
		);

		return DataAccessor.inject(
			listOfMetaAccessors,
			thisAccessor.dataStorage
		);
	};

	// @param this - current accessor
	var metaListBy = function(value){
		var result;

		if(typeof(value) == "function"){

			result = metaListByPredicateFunction.call(this, value);
		}
		else{

			result = metaListByMetaDate.call(this, value);
		}

		return result;
	};


	/**
	 * @param this - current accessor
	 * @param predicateFunction
	 * 	this - current accessor
	 * 	index - index within list of meta data for a date
	 * 	metaElement - accessor for current meta date
	**/
	var metaListByPredicateFunction = function(predicateFunction){

		var thisAccessor = this;

		var resultIndices = thisAccessor.map(
			function(element){

				if(Array.isArray(element)){

					return element.meta(predicateFunction);
				}
				else{

					var result = undefined;

					var metaDataIndices = thisAccessor.dataStorage.metaDataRelation[element];

					for(var index = 0; index < metaDataIndices.length; index++){

						var metaDate = DataAccessor.inject(
							[metaDataIndices[index]],
							thisAccessor.dataStorage
						);

						var fulfiledPredicate = predicateFunction.call(
							thisAccessor,
							index,
							metaDate
						);

						if(fulfiledPredicate){

							result = metaDataIndices[index];
							break;
						}
					}

					return result;
				}
			}
		);

		return DataAccessor.inject(resultIndices, thisAccessor.dataStorage);
	};

	// @param this - current accessor
	var metaListByMetaDate = function(metaMetaDate){

		return metaListByPredicateFunction.call(
			this,
			function(index, metaElement){

				var metaMetaData = metaElement.meta()[0].access();

				// "return metaMetaData.contains(metaMetaDate);"
				return metaMetaData.indexOf(metaMetaDate) != -1;
			}
		);
	};

	//############
	//### main ###
	//############
	var result;

	if(arguments.length == 0){
		result = metaLists.call(this);
	}
	else{
		result = metaListBy.call(this, value);
	}

	return result;
}
