const input = document.getElementById("problemInput");
const button = document.getElementById("exploreButton");
const responseArea = document.getElementById("responseArea");

button.addEventListener("click", async function () {

    const problem = input.value.trim();

    // Make sure the user entered something
    if (problem === "") {
        responseArea.innerHTML = 
            "<p>Please enter something you'd like to explore.</p>";
        input.focus();
        return;
    }

    // Show loading state
    button.disabled = true;
    button.textContent = "Thinking...";
    
    responseArea.innerHTML = 
        "<p>Exploring your problem mathematically...</p>";

    try {

        // Send the question to our C++ backend
        const result = await fetch("/ask", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                problem: problem
            })
        });

        if (!result.ok) {
            throw new Error("Server error");
        }

        const data = await result.json();

        // Display the AI response
        responseArea.innerHTML = "";

        const answer = document.createElement("div");
        answer.textContent = data.answer;
        answer.style.whiteSpace = "pre-wrap";

        responseArea.appendChild(answer);

    } catch (error) {

        responseArea.innerHTML = 
            "<p>Your question is ready! We still need to connect " +
            "the website to your C++ AI engine. That's our next step.</p>";

    }

    // Restore the button
    button.disabled = false;
    button.textContent = "Explore mathematically ↗";

});

// Allow Command + Enter to submit
input.addEventListener("keydown", function (event) {

    if (event.metaKey && event.key === "Enter") {
        button.click();
    }

});