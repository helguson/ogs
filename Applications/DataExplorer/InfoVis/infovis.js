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

	var sampleSVG = d3.select("#viz")
		.append("svg")
		.attr("width", 100)
		.attr("height", 100);

	sampleSVG.append("circle")
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
	sampleSVG.selectAll("circle").on(
		"click",
		function(){
			qtPushButton.text = "Javascript takes over!";
		}
	);
}
