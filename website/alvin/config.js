/* 
 * Alvin IFTTT Control Application
 *
 * Copyright (c) 2015 James Fowler
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*******************************************************************************
 * 
 * Main process
 * 
 ******************************************************************************/

$("document").ready(function() {

  // Adjust page for viewport
  adjustForViewport();

  // Spot if we are on iOS or not
  document.ios = navigator.userAgent.match(/iPhone/i) || navigator.userAgent.match(/iPad/i) || navigator.userAgent.match(/iPod/i);

  if (document.ios) {
    $(".android").hide();
  } else {
    $(".ios").hide();
  }

  // Pick up parameters from URL
  var vers = getParameterByName("vers");
  var key = decodeURIComponent(getParameterByName("key"));
  var menuName1 = decodeURIComponent(getParameterByName("mn1"));
  var menuName2 = decodeURIComponent(getParameterByName("mn2"));
  var menuName3 = decodeURIComponent(getParameterByName("mn3"));
  var menuName4 = decodeURIComponent(getParameterByName("mn4"));
  var menuName5 = decodeURIComponent(getParameterByName("mn5"));
  var menuName6 = decodeURIComponent(getParameterByName("mn6"));
  var menuName7 = decodeURIComponent(getParameterByName("mn7"));
  var menuName8 = decodeURIComponent(getParameterByName("mn8"));
  var menuName9 = decodeURIComponent(getParameterByName("mn9"));
  var menuName10 = decodeURIComponent(getParameterByName("mn10"));
  var toggle1 = getParameterByName("t1");
  var toggle2 = getParameterByName("t2");
  var toggle3 = getParameterByName("t3");
  var toggle4 = getParameterByName("t4");
  var toggle5 = getParameterByName("t5");
  var toggle6 = getParameterByName("t6");
  var toggle7 = getParameterByName("t7");
  var toggle8 = getParameterByName("t8");
  var toggle9 = getParameterByName("t9");
  var toggle10 = getParameterByName("t10");
  var returnTo = getParameterByName("return_to");
  if (returnTo === "") {
    returnTo = "pebblejs://close#";
  }

  // Set screen fields
  $("#key").val(key);
  $("#menuName1").val(menuName1);
  $("#menuName2").val(menuName2);
  $("#menuName3").val(menuName3);
  $("#menuName4").val(menuName4);
  $("#menuName5").val(menuName5);
  $("#menuName6").val(menuName6);
  $("#menuName7").val(menuName7);
  $("#menuName8").val(menuName8);
  $("#menuName9").val(menuName9);
  $("#menuName10").val(menuName10);
  $("#toggle1").prop("checked", toggle1 === "Y");
  $("#toggle2").prop("checked", toggle2 === "Y");
  $("#toggle3").prop("checked", toggle3 === "Y");
  $("#toggle4").prop("checked", toggle4 === "Y");
  $("#toggle5").prop("checked", toggle5 === "Y");
  $("#toggle6").prop("checked", toggle6 === "Y");
  $("#toggle7").prop("checked", toggle7 === "Y");
  $("#toggle8").prop("checked", toggle8 === "Y");
  $("#toggle9").prop("checked", toggle9 === "Y");
  $("#toggle10").prop("checked", toggle10 === "Y");
  
  // Set version
  $("#version").text(parseInt(vers, 10) / 10);

  // Show version warning
  setScreenMessageBasedOnVersion(vers);
  
  // Sometimes we ask for love or donations
  if ((new Date().valueOf()) % 10 === 0) {
    $("#info-message").css("display", "block");
  }

  // Move down
  $(".downicon").click(function() {
    var tr = $(this).parent().parent();
    var nexttr = tr.next();

    // Read
    var currentText = $(".namefield", tr).val();
    var nextText = $(".namefield", nexttr).val();
    var currentToggle = $(".togglechk", tr).is(':checked');
    var nextToggle = $(".togglechk", nexttr).is(':checked');

    // Write
    $(".namefield", tr).val(nextText);
    $(".namefield", nexttr).val(currentText);
    $(".togglechk", tr).prop("checked", nextToggle);
    $(".togglechk", nexttr).prop("checked", currentToggle);
  });

  // Move up
  $(".upicon").click(function() {
    var tr = $(this).parent().parent();
    var prevtr = tr.prev();

    // Read
    var currentText = $(".namefield", tr).val();
    var prevText = $(".namefield", prevtr).val();
    var currentToggle = $(".togglechk", tr).is(':checked');
    var prevToggle = $(".togglechk", prevtr).is(':checked');

    // Write
    $(".namefield", tr).val(prevText);
    $(".namefield", prevtr).val(currentText);
    $(".togglechk", tr).prop("checked", prevToggle);
    $(".togglechk", prevtr).prop("checked", currentToggle);
  });

  // Handle the Save and reset option
  $(".save").click(function() {
    var configData = {
      action : "save",
      key : safeTrim($("#key").val()),
      mn1 : safeTrim($("#menuName1").val()),
      mn2 : safeTrim($("#menuName2").val()),
      mn3 : safeTrim($("#menuName3").val()),
      mn4 : safeTrim($("#menuName4").val()),
      mn5 : safeTrim($("#menuName5").val()),
      mn6 : safeTrim($("#menuName6").val()),
      mn7 : safeTrim($("#menuName7").val()),
      mn8 : safeTrim($("#menuName8").val()),
      mn9 : safeTrim($("#menuName9").val()),
      mn10 : safeTrim($("#menuName10").val()),
      t1 : $("#toggle1").is(':checked') ? "Y" : "N",
      t2 : $("#toggle2").is(':checked') ? "Y" : "N",
      t3 : $("#toggle3").is(':checked') ? "Y" : "N",
      t4 : $("#toggle4").is(':checked') ? "Y" : "N",
      t5 : $("#toggle5").is(':checked') ? "Y" : "N",
      t6 : $("#toggle6").is(':checked') ? "Y" : "N",
      t7 : $("#toggle7").is(':checked') ? "Y" : "N",
      t8 : $("#toggle8").is(':checked') ? "Y" : "N",
      t9 : $("#toggle9").is(':checked') ? "Y" : "N",
      t10 : $("#toggle10").is(':checked') ? "Y" : "N"
    };
    var move = true;
    while (move) {
      move = false;

      if (configData.mn1 === "" && configData.mn2 !== "") {
        configData.mn1 = configData.mn2;
        configData.t1 = configData.t2;
        configData.mn2 = "";
        configData.t2 = "N";
        move = true;
      }
      if (configData.mn2 === "" && configData.mn3 !== "") {
        configData.mn2 = configData.mn3;
        configData.t2 = configData.t3;
        configData.mn3 = "";
        configData.t3 = "N";
        move = true;
      }
      if (configData.mn3 === "" && configData.mn4 !== "") {
        configData.mn3 = configData.mn4;
        configData.t3 = configData.t4;
        configData.mn4 = "";
        configData.t4 = "N";
        move = true;
      }
      if (configData.mn4 === "" && configData.mn5 !== "") {
        configData.mn4 = configData.mn5;
        configData.t4 = configData.t5;
        configData.mn5 = "";
        configData.t5 = "N";
        move = true;
      }
      if (configData.mn5 === "" && configData.mn6 !== "") {
        configData.mn5 = configData.mn6;
        configData.t5 = configData.t6;
        configData.mn6 = "";
        configData.t6 = "N";
        move = true;
      }
      if (configData.mn6 === "" && configData.mn7 !== "") {
        configData.mn6 = configData.mn7;
        configData.t6 = configData.t7;
        configData.mn7 = "";
        configData.t7 = "N";
        move = true;
      }
      if (configData.mn7 === "" && configData.mn8 !== "") {
        configData.mn7 = configData.mn8;
        configData.t7 = configData.t8;
        configData.mn8 = "";
        configData.t8 = "N";
        move = true;
      }
      if (configData.mn8 === "" && configData.mn9 !== "") {
        configData.mn8 = configData.mn9;
        configData.t8 = configData.t9;
        configData.mn9 = "";
        configData.t9 = "N";
        move = true;
      }
      if (configData.mn9 === "" && configData.mn10 !== "") {
        configData.mn9 = configData.mn10;
        configData.t9 = configData.t10;
        configData.mn10 = "";
        configData.t10 = "N";
        move = true;
      }
    }
    document.location = returnTo + encodeURIComponent(JSON.stringify(configData));
  });

});
