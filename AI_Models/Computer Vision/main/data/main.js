document.addEventListener('DOMContentLoaded', function() {
    const streamImg = document.getElementById('stream');
    
    streamImg.src = window.location.protocol + "//" + window.location.hostname + ":81/stream";
    
   
    streamImg.onerror = function() {
        console.log("Stream lost, retrying...");
        setTimeout(() => {
            streamImg.src = window.location.protocol + "//" + window.location.hostname + ":81/stream?t=" + new Date().getTime();
        }, 1000);
    };
});


setInterval(async () => {
    try {
     
        const res = await fetch('http://' + location.hostname + ':82');
        const data = await res.json();
        
    
        document.getElementById('label').innerText = data.label;
        document.getElementById('score').innerText = data.score + '%';

        
        const labelDiv = document.getElementById('label');
        if(data.label === "Safe Driving" || data.label === "safe driving") {
             labelDiv.style.color = "#4ade80"; 
        } else if (data.score > 50) {
             labelDiv.style.color = "#ef4444"; 
        } else {
             labelDiv.style.color = "#ffffff";
        }

    } catch(e) { 
        console.log("Waiting for API...", e); 
    }
}, 500);