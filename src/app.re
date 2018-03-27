[%bs.raw {|require('./app.css')|}];

let str = ReasonReact.stringToElement;

let component = ReasonReact.statelessComponent("App");

let someJson = {|{
  "hello": "world"
}|};

type someJsonType = {. "hello": string};

[@bs.scope "JSON"] [@bs.val]
external parseSomeJson : string => someJsonType = "parse";

/* type someJsonType = {. hello: string}; */
let someObject = someJson |> parseSomeJson;

module Option = {
  let unwrapUnsafely = data =>
    switch (data) {
    | Some(v) => v
    | None => raise(Invalid_argument("unwrapUnsafely called on None"))
    };
};

type user = {
  id: int,
  name: string,
  address,
}
and address = {city: string};

module Decode = {
  let address = json => Json.Decode.{city: json |> field("city", string)};
  let user = json =>
    Json.Decode.{
      id: json |> field("id", int),
      name: json |> field("name", string),
      address: json |> field("address", address),
    };
  let users = json => Json.Decode.array(user, json);
};

module Encode = {
  let address = v =>
    Json.Encode.object_([("city", v.city |> Json.Encode.string)]);
  let user = v =>
    Json.Encode.object_([
      ("id", v.id |> Json.Encode.int),
      ("name", v.name |> Json.Encode.string),
      ("address", v.address |> address),
    ]);
  let users = v => Json.Encode.array(user, v);
};

let fetchUsers = () =>
  Js.Promise.(
    Fetch.fetch("https://jsonplaceholder.typicode.com/users")
    |> then_(Fetch.Response.json)
    |> then_(json => Decode.users(json) |> resolve)
  );

let handleClick = _event => {
  fetchUsers()
  |> Js.Promise.then_(usersList => {
       let firstUser = usersList[0];
       Js.log(firstUser.name ++ " of " ++ firstUser.address.city);
       Js.Promise.resolve(usersList);
     })
  |> Js.Promise.then_(usersList => {
       Encode.users(usersList) |> Js.Json.stringify |> Js.log;
       Js.Promise.resolve();
     });
  Js.log("All set up!");
};

let make = _children => {
  ...component,
  render: _self =>
    <div>
      <div className="app__header">
        <h2> (str("Let's play with JSON in ReasonML!")) </h2>
      </div>
      <div className="app__content">
        <p> (str("Let's start with some JSON:")) </p>
        <pre> (str(someJson)) </pre>
        <p>
          (str("We'll start easy by writing an unsafe call to JSON.parse"))
        </p>
        <p>
          (
            str(
              "But what if you're not holding a string. What can you do if ReasonML hands you a <code>Js.Json.t</code> type object? In fact, this is exactly what happens if you use <code>fetch</code>, and parse the response as JSON. The promise will resolve in a <code>Js.Json.t</code> which you can't use until you decode it. In such situations, your best option is to play it safe, and use <code>bs-json</code> library.",
            )
          )
        </p>
        <p>
          (
            str(
              "We'll use a library called bs-json to convert the above string into a Bucklescript Object (as \"Record\").",
            )
          )
        </p>
        <p>
          (
            str(
              "Before we do so, we must create a type that represents this data. We'll need it for the conversion.",
            )
          )
        </p>
        (str(someObject##hello))
        <button onClick=handleClick> (str("Load Users from API")) </button>
      </div>
    </div>,
};