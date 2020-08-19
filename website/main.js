$(document).ready(function () {
    $("a").hover(
        function () {
            $(this).css("color", getRandomColor());
        },
        function () {
            $(this).css("color", "");
        }
    );

    function getRandomColor() {
        var letters = "0123456789ABCDEF".split("");
        var color = "#";
        for (var i = 0; i < 6; i++) {
            color += letters[Math.floor(Math.random() * 16)];
        }
        return color;
    }
});