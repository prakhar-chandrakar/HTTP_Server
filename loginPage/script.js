const loginForm = document.querySelector(".login-form");

loginForm.addEventListener("submit", async (e) => {
  e.preventDefault();
  const username = document.getElementById("username").value;
  const password = document.getElementById("password").value;
  try {
    const requestOptions = {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        "username": username,
        "password": password,
      }),
    };
    fetch("http://127.0.0.1:8080/", requestOptions)
      .then((response) => response.text())
      .then((result)=> {
        console.log(result);
        alert(result);
      });

    document.getElementById("username").value = "";
    document.getElementById("password").value = "";
  } catch (err) {
    alert(err);
  }
});
