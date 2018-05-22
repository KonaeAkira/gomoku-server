var SIZE = 15;
var COND = 5;

var moves = [];
var currentMove = 0;
var status = "OFFLINE";

var time = 0;

function updateTimer() {
	++time;
	if (time % 10 == 0) {
		$("#timer").html(time / 10 + ".0");
	} else {
		$("#timer").html("" + time/10);	
	}
}

function resetTimer() { time = 0; }


function drawBoard() {
	for (var i = 0; i < SIZE; ++i) {
		$(".board").append("<tr class=\"boardRow\" id=\"boardRow_" + i + "\"></tr>");
		for (var j = 0; j < SIZE; ++j) {
			$("#boardRow_" + i).append("<td class=\"boardCol\" id=\"boardCol_" + i + "_" + j + "\"><div class=\"boardCell\" id=\"boardCell_" + i + "_" + j + "\"></div></td>");
		}
	}
}

function updateUI() {
	$("td .boardCellBlack").removeClass("boardCellBlack");
	$("td .boardCellWhite").removeClass("boardCellWhite");
	var color = ["boardCellBlack", "boardCellWhite"];
	for (var i = 0; i < currentMove; ++i) {
		$("#boardCell_" + moves[i][0] + "_" + moves[i][1]).addClass(color[i & 1]);
	}
	$("#status").html(status);
}

function pollStatus() {
	$.get("/request/status", function(response) { 
		status = response;
	});
}

function pollMove() {
	$.get("/request/move/" + moves.length, function(response) { 
		if (response != "NIL") {
			if (currentMove == moves.length) {
				++currentMove;
			}
			pollStatus();
			moves.push(response.split(","));
			resetTimer();
			updateUI();
		}
	});
}

$(document).ready(function() {
	// initialize the board
	drawBoard();
	// get black player's name
	$.get("/request/name/black", function(response) {
		$("#nameBlack").html(response);
	});
	// get white player's name
	$.get("/request/name/black", function(response) {
		$("#nameWhite").html(response);
	});
	// poll initial status
	pollStatus();
	// start polling for new moves
	window.setInterval(pollMove, 200);
	window.setInterval(updateTimer, 100);
	// key controls
	$(document).keydown(function(event) {
		switch (event.which) {
			case 37: // left arrow
				--currentMove;
				if (currentMove < 0) {
					currentMove = 0;
				}
				updateUI();
				break;
			case 39: // right arrow
				++currentMove;
				if (currentMove > moves.length) {
					currentMove = moves.length;
				}
				updateUI();
				break;
		}
	});
});
