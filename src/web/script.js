var SIZE = 15;
var COND = 5;

$(document).ready(function() {
	for (var i = 0; i < SIZE; ++i) {
		$(".board").append("<tr class=\"boardRow\" id=\"boardRow_" + i + "\"></tr>");
		for (var j = 0; j < SIZE; ++j) {
			$("#boardRow_" + i).append("<td class=\"boardCol\" id=\"boardCol_" + i + "_" + j + "\"><div class=\"boardCell\" id=\"boardCell_" + i + "_" + j + "\"></div></td>");
		}
	}
});
