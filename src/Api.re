type wordJson = {
  start: Utils.coords,
  dir: string,
  hint: string,
  answer: string
};

let decode_coords = fun(coordsJson) => {
  Json.Decode.{
    Utils.row: coordsJson |> field("row", int),
    col: coordsJson |> field("col", int)
  }
};

let parse_json =
  Json.Decode.(array((json) => {
    Utils.start: json |> field("start", decode_coords),
    dir: json |> field("dir", (tDir) => string(tDir) === "R" ? Utils.Right : Utils.Down),
    hint: json |> field("hint", string),
    answer: json |> field("answer", string)
  }));

let get_data = fun() => {
  Js.Promise.(
    Fetch.fetch("http://localhost:1235/")
    |> then_(Fetch.Response.json)
    |> then_((json) => parse_json(json) |> resolve)
  );
};
