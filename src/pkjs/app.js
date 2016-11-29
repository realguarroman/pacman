Pebble.addEventListener("showConfiguration",
  function(e) {
    //Load the remote config page
    Pebble.openURL("http://www.azucarconamor.es/pebble/docs/config_pachealth.html");
  }
);


Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));
 
    //Send to Pebble, persist there
		
		var dictionary = {
				"KEY_CHARACTER": configuration.character,
			
			
		};
		
		
    Pebble.sendAppMessage(dictionary,
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!");
      }
    );
  }
);


