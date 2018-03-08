type state = { words: Utils.loadable(Utils.words, string), grid: option(Utils.grid) };
type action =
  | LoadStarted
  | LoadFinished(Utils.words);

let component = ReasonReact.reducerComponent("App");

let make = (_children) => {
  ...component,
  initialState: () => { words: None, grid: None },
  reducer: (action, _) =>
    switch (action) {
    | LoadStarted => ReasonReact.Update({ words: Utils.Loading, grid: None })
    | LoadFinished(w) => {
      Js.log(w);
      ReasonReact.Update({ words: Utils.Loaded(w), grid: Some(Utils.build_full_grid(w)) });
    }
    },
  didMount: (self) => {
    Api.get_data() |> Js.Promise.then_(value => {
      self.send(LoadFinished(value));
      Js.Promise.resolve(0);
    }) |> ignore;
    ReasonReact.NoUpdate;
  },
  render: (self) => {
    switch (self.state.words, self.state.grid) {
      | (Loaded(w), Some(g)) => <Grid grid={g} words={w} />
      | (Loading, _) => <div>{ReasonReact.stringToElement("Loading...")}</div>
      | (Error(err), _) => <div>{ReasonReact.stringToElement(err)}</div>
      | _ => <div>{ReasonReact.stringToElement("Nothing to see.")}</div>
    };
  }
};
