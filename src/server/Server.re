open Express;

let words = "[\n  { \"start\": { \"row\": 0, \"col\": 1 }, \"dir\": \"R\", \"hint\": \"\", \"answer\": \"EGGS\" },\n  { \"start\": { \"row\": 2, \"col\": 0 }, \"dir\": \"R\", \"hint\": \"\", \"answer\": \"CHEESE\" },\n  { \"start\": { \"row\": 6, \"col\": 0 }, \"dir\": \"R\", \"hint\": \"\", \"answer\": \"BROWN\" },\n  { \"start\": { \"row\": 0, \"col\": 2 }, \"dir\": \"D\", \"hint\": \"\", \"answer\": \"GEESE\" },\n  { \"start\": { \"row\": 0, \"col\": 4 }, \"dir\": \"D\", \"hint\": \"\", \"answer\": \"SUSTAIN\" },\n  { \"start\": { \"row\": 2, \"col\": 0 }, \"dir\": \"D\", \"hint\": \"\", \"answer\": \"CLIMB\" },\n  { \"start\": { \"row\": 4, \"col\": 2 }, \"dir\": \"R\", \"hint\": \"\", \"answer\": \"ELAN\" }\n]";

let (>>) = (f, g, x) => x |> f |> g;

let app = express();

App.get(
  app,
  ~path="/",
  Middleware.from(
    (_, req) => {
      Js.log(Request.bodyRaw(req));
      Response.sendString(words)
    }
  )
);

App.listen(app, ~port=1235, ());
