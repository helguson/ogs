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

	this.getAccessorFor = function(indexList){

		return new DataAccessor(indexList, this);
	};
}

infovis.DataAccessor = function(indexList, dataStorage){

	var DataAccessor = infovis.DataAccessor;

	var indexList = indexList;
	var dataStorage = dataStorage;
	var thisAccessor = this;

	//#######################
	//### private methods ###
	//#######################
	var createNewDataAccessor = function(indexList){

		return new DataAccessor(
			indexList,
			dataStorage
		);
	}

	var accessList = function(){

		return indexList.map(
			function(valueIndex){

				return dataStorage.values[valueIndex];
			}
		)
	};

	var accessElement = function(value){
		var result;

		if(typeof(value) == "function"){

			result = accessElementByPredicateFunction(value);
		}
		else{

			result = accessElementByMetaDate(value);
		}

		return result;
	};

	var accessElementByMetaDate = function(metaDate){

		return accessElementByPredicateFunction(
			function(index, element){

				var metaData = element.meta()[0].access();

				return metaData.indexOf(metaDate) != -1;
			}
		);
	};

	/**
	 * @brief returns first value for that the predicate function returns true
	 * @param predicateFunction(index, element)
	 * 	this - current accessor
	 * 	index - index of element within the current accessor
	 * 	element - accessor for the current element
	**/
	var accessElementByPredicateFunction = function(predicateFunction){

		var result = undefined;

		for(var indexListIndex = 0; indexListIndex < indexList.length; indexListIndex++){

			var elementAccessor = createNewDataAccessor(
				[indexList[indexListIndex]]
			);

			var fulfiledPredicate = predicateFunction.call(
				thisAccessor,
				indexListIndex,
				elementAccessor
			);

			if(fulfiledPredicate){

				result = elementAccessor.access()[0];
				break;
			}
		}

		return result;
	};

	var metaLists = function(){

		return indexList.map(
			function(valueIndex){

				return createNewDataAccessor(
					dataStorage.metaDataRelation[valueIndex]
				);
			}
		);
	};

	var metaListBy = function(value){
		var result;

		if(typeof(value) == "function"){

			result = metaListByPredicateFunction(value);
		}
		else{

			result = metaListByMetaDate(value);
		}

		return result;
	};


	/**
	 * @param predicateFunction
	 * 	this - current accessor
	 * 	index - index within list of meta data for a date
	 * 	metaElement - accessor for current meta date
	**/
	var metaListByPredicateFunction = function(predicateFunction){

		var resultIndices = indexList.map(
			function(valueIndex){
				var result = -1;

				var metaDataIndices = dataStorage.metaDataRelation[valueIndex];

				for(var index = 0; index < metaDataIndices.length; index++){

					var metaDate = createNewDataAccessor(
						[metaDataIndices[index]]
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
		);

		return createNewDataAccessor(
			resultIndices
		);
	};

	var metaListByMetaDate = function(metaMetaDate){

		return metaListByPredicateFunction(
			function(index, metaElement){

				var metaMetaData = metaElement.meta()[0].access();

				return metaMetaData.indexOf(metaMetaDate) != -1;
			}
		);
	};

	//######################
	//### public methods ###
	//######################
	this.access = function(value){
		var result;

		if(arguments.length == 0){
			result = accessList();
		}
		else{
			result = accessElement(value);
		}

		return result;
	};

	this.meta = function(value){
		var result;

		if(arguments.length == 0){
			result = metaLists();
		}
		else{
			result = metaListBy(value);
		}

		return result;
	};

	this.prepareJoin = function(){

	};
}
