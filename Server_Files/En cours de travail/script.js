function refreshPage() {
	location.reload();
}

function readtime() {
	var now = new Date();
	
	var annee   = now.getFullYear();
	var mois    = now.getMonth() + 1;
	var jour    = now.getDate();
	var heure   = now.getUTCHours();
	var minute  = now.getUTCMinutes();
	var seconde = now.getUTCSeconds();
	
	alert( "UTC Time (from computer) : "+jour+"/"+mois+"/"+annee+" "+heure+":"+minute+":"+seconde);
		
}

setInterval(function readUTCtime() 
{	
    var dateAndTime = document.getElementById('dateAndTime');  	
	var displayDay = document.getElementById('displayDay');
    var displayMonth = document.getElementById('displayMonth');
    var displayYear = document.getElementById('displayYear');	
	var displayHours = document.getElementById('displayHours');
    var displayMinutes = document.getElementById('displayMinutes');
    var displaySeconds = document.getElementById('displaySeconds');	
	
	var nowUTC = new Date();
	
	var yearUTC = nowUTC.getFullYear();
	var monthUTC = nowUTC.getMonth() + 1;
	var dayUTC = nowUTC.getDate();
	var hoursUTC = nowUTC.getUTCHours();
	var minutesUTC = nowUTC.getUTCMinutes();
	var secondsUTC = nowUTC.getUTCSeconds();

	if (secondsUTC <10) {
		secondsUTC = "0"+ secondsUTC;
	}
	
	dateAndTime.innerHTML = nowUTC;   
    displayYear.innerHTML = yearUTC;
    displayMonth.innerHTML = monthUTC;
    displayDay.innerHTML = dayUTC;    
	displayHours.innerHTML = hoursUTC;
    displayMinutes.innerHTML = minutesUTC;
    displaySeconds.innerHTML = secondsUTC;

},1000);


setInterval(function getData()
{
    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("valeurSeconds").innerHTML = this.responseText;
        }
    };

    xhttp.open("GET", "lireValue", true);
    xhttp.send();
}, 1000);


function synctime() {
	
	
	
}

