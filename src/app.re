[%bs.raw {|require('./app.css')|}];

let str = ReasonReact.stringToElement;

let component = ReasonReact.statelessComponent("App");

let someJson = {|{
  "hello": "world"
}|};

/* ************** */
/* Unsafe parsing */
/* ************** */
type someJsonType = {. "hello": string};

[@bs.scope "JSON"] [@bs.val]
external parseSomeJson : string => someJsonType = "parse";

let someObject = someJson |> parseSomeJson;

/* **************************** */
/* Decode & Encode with bs-json */
/* **************************** */
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
     })
  |> ignore;
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
        <h3> (str("Unsafe parsing")) </h3>
        <p>
          (str("Let's start with some JSON (string) stored in a variable:"))
        </p>
        <pre>
          (
            str(
              {string|let someJson = {|
{
  "hello": "world"
}
|};
          |string},
            )
          )
        </pre>
        <p
          dangerouslySetInnerHTML={
            "__html": "We'll start easy by writing an unsafe call to <code>JSON.parse</code>.",
          }
        />
        <pre>
          (
            str(
              {|type someJsonType = {. "hello": string};

[@bs.scope "JSON"] [@bs.val]
external unsafeParse : string => someJsonType = "parse";

let parsedRecord = someJson |> unsafeParse;|},
            )
          )
        </pre>
        <p
          dangerouslySetInnerHTML={
            "__html": "Here, you're telling Reason to <em>trust</em> you that the return from <code>JSON.parse</code> is going to be of the specified type. It works, but it's not safe for <em>most</em> real-world use cases.",
          }
        />
        <p
          dangerouslySetInnerHTML={
            "__html": "But what if you're not holding a string. What can you do if ReasonML hands you a <code>Js.Json.t</code> type object? In fact, this is exactly what happens if you use <code>fetch</code>, and parse the response as JSON. The promise will resolve in a <code>Js.Json.t</code> which you can't use until you decode it. In such situations, your best option is to play it safe, and use <code>bs-json</code> library.",
          }
        />
        <h3> (str("Fetch and Parse JSON response")) </h3>
        <p
          dangerouslySetInnerHTML={
            "__html": "Let's <em>fetch</em> some JSON with <code>bs-fetch</code> and parse and decode it with <code>bs-json</code>!",
          }
        />
        <button className="app__load-button" onClick=handleClick>
          (str("Click here to load users from the API"))
        </button>
      </div>
    </div>,
};