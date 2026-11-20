async function analyzeProblem() {
    const problemInput = document.getElementById("problem");
    const button = document.getElementById("analyzeButton");
    const responseArea = document.getElementById("responseArea");

    const problem = problemInput.value.trim();

    // Make sure the user entered something
    if (problem === "") {
        responseArea.textContent = "Please enter a problem first.";
        return;
    }

    // Show loading state
    button.disabled = true;
    button.textContent = "Exploring the mathematics...";
    responseArea.textContent = "Thinking through your situation...";

    try {
        // Send the problem to the C++ backend
        const response = await fetch("/ask", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                problem: problem
            })
        });

        // Read the server's response
        const data = await response.json();

        // Check for server errors
        if (!response.ok) {
            throw new Error(
                data.error || "Something went wrong with the server."
            );
        }

        // Display the AI response
       const answer =
    data.answer ||
    data.response ||
    data.message ||
    "The server responded, but no answer was returned.";

responseArea.innerHTML = DOMPurify.sanitize(
    marked.parse(answer)
);

if (window.MathJax) {
    MathJax.typesetPromise([responseArea]);
}

    } catch (error) {
        console.error("Error:", error);

        responseArea.textContent =
            "Something went wrong. Please try again.\n\n" +
            error.message;

    } finally {
        // Restore the button
        button.disabled = false;
        button.textContent = "Explore the mathematics";
    }
}
