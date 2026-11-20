 },
            body: JSON.stringify({ problem: problem })
        });

        const result = await response.json();

        if (!response.ok) {
            throw new Error(result.error || "Request failed.");
        }

        responseArea.textContent = result.answer || result.response || JSON.stringify(result);

    } catch (error) {
        responseArea.textContent = "Error: " + error.message;
        console.error("Future Function error:", error);

    } finally {
        button.disabled = false;
        button.textContent = "Explore the mathematics";
    }
}
