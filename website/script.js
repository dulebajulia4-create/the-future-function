async function analyzeProblem() {
    const problem = document.getElementById("problem").value.trim();
    const button = document.getElementById("analyzeButton");
    const responseArea = document.getElementById("responseArea");

    if (!problem) {
        responseArea.textContent = "Please enter a problem first.";
        return;
    }

    button.disabled = true;
    button.textContent = "Exploring the mathematics...";
    responseArea.textContent = "Thinking through your situation...";

    try {
        const response = await fetch("/ask", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({ problem: problem })
        });

        const result = await response.json();

        if (!response.ok) {
            throw new Error(result.error || "Request failed.");
        }

        responseArea.textContent =
            result.answer || result.response || JSON.stringify(result);

    } catch (error) {
        responseArea.textContent = "Error: " + error.message;
        console.error("Future Function error:", error);

    } finally {
        button.disabled = false;
        button.textContent = "Explore the mathematics";
    }
}
