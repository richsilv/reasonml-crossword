type state = { clues: Utils.loadable(Utils.clues, string), grid: option(Utils.grid) };
type action =
  | LoadStarted
  | LoadFinished(Utils.words);

let component = ReasonReact.reducerComponent("App");

let make = (_children) => {
  ...component,
  initialState: () => { clues: None, grid: None },
  reducer: (action, _) =>
    switch (action) {
    | LoadStarted => ReasonReact.Update({ clues: Utils.Loading, grid: None })
    | LoadFinished(w) => {
      Js.log(w);
      ReasonReact.Update({ clues: Utils.Loaded(Utils.make_clues(w)), grid: Some(Utils.build_full_grid(w)) });
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
    switch (self.state.clues, self.state.grid) {
      | (Loaded(c), Some(g)) => <Grid vGrid={g} vClues={c} />
      | (Loading, _) => <div>{ReasonReact.stringToElement("Loading...")}</div>
      | (Error(err), _) => <div>{ReasonReact.stringToElement(err)}</div>
      | _ => <div>{ReasonReact.stringToElement("Nothing to see.")}</div>
    };
  }
};
