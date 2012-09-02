oneliness.onFirefoxLoad = function(event) {
  document.getElementById("contentAreaContextMenu")
          .addEventListener("popupshowing", function (e){ oneliness.showFirefoxContextMenu(e); }, false);
};

oneliness.showFirefoxContextMenu = function(event) {
  // show or hide the menuitem based on what the context menu is on
  document.getElementById("context-oneliness").hidden = gContextMenu.onImage;
};

window.addEventListener("load", oneliness.onFirefoxLoad, false);
