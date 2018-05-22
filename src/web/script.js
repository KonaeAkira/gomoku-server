var SIZE = 15;
var COND = 5;

var moves = [];
var currentMove = 0;
var status = "OFFLINE";

var time = 0;

function updateTimer() {
	if (time == -1) {
		return;
	}
	++time;
	if (time % 10 == 0) {
		$("#timer").html(time / 10 + ".0");
	} else {
		$("#timer").html("" + time/10);	
	}
}

function resetTimer() { time = 0; }
function stopTimer() { time = -1; }

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
	for (var i = 0; i < currentMove; ++i) {
		if (moves[i][0] == "BLACK") {		
			$("#boardCell_" + moves[i][1] + "_" + moves[i][2]).addClass("boardCellBlack");
		} else {
			$("#boardCell_" + moves[i][1] + "_" + moves[i][2]).addClass("boardCellWhite");
		}
	}	
}

function initSocket() {
	var socket = new WebSocket("ws://" + location.host);
	socket.onmessage = function(event) {
		console.log(event.data);
		var message = event.data.split(",");
		if (message[0] == "PLAYER") {
			if (message[1] == "BLACK") {
				$("#nameBlack").html(message[2]);
			} else {
				$("#nameWhite").html(message[2]);
			}
		} else if (message[0] == "MOVE") {
			if (currentMove == moves.length) {
				++currentMove;
			}
			moves.push([message[1], message[2], message[3]]);
			resetTimer(); updateUI();
		} else if (message[0] == "STATUS") {
			$("#status").html(message[1]);
			if (message[1].split("_")[0] != "TURN") {
				stopTimer();
			}
		}
	}
}

$(document).ready(function() {
	// initialize the board
	drawBoard();
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
	initSocket();
});
