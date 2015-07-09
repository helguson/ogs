
// define infovis package / conglomerate- / accumulation-object
if(window.infovis == undefined){
	window.infovis = {};
}

(function(container){

container.Interval = function(lowerLimit, lowerLimitIsInclusive, upperlimit, upperLimitIsInclusive){

	this._lowerLimit = lowerLimit;
	this._lowerLimitIsInclusive = lowerLimitIsInclusive;
	this._upperlimit = upperlimit;
	this._upperLimitIsInclusive = upperLimitIsInclusive;

	return this;
}

container.Interval.prototype.toString = function(){

	var result = "";

	result += this._lowerLimitIsInclusive ? "[" : "(";
	result += this._lowerLimit.toString();
	result += ", ";
	result += this._upperlimit.toString();
	result += this._upperLimitIsInclusive ? "]" : ")";

	return result;
}

})(infovis);
