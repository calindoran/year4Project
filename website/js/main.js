$(document).ready(function () {
  $("a").hover(function () {
    $(this).css("color", randCol());
  });

  function randCol() {
    var letters = "0123456789ABCDEF".split("");
    var color = "#";

    for (var i = 0; i < 6; i++) {
      color += letters[Math.floor(Math.random() * 16)];
    }
    return color;
  }
});

// not used, but handy for rand color
