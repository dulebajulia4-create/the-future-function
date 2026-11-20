  problem: problem
            })
        });

        const data = await response.json();

        if (!response.ok) {
            throw new Error(data.error || "Something went wrong.");
        }

        responseArea.textContent = data.response;

    } catch (error) {
        responseArea.textContent =
            "Something went wrong. Please try again.\n\n" + error.message;

    } finally {
        button.disabled = false;
        button.textContent = "Explore the mathematics";
    }
}
