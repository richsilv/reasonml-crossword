let component = ReasonReact.statelessComponent("Cell");

let make = (~value: Utils.contents, ~vClue: option(Utils.clue), _children) => {
  ...component,
  render: (_self) => <div> {
    switch (value) {
      | Empty => <div className="cell" />
      | FilledIn => <div className="cell filled-in" />
      | Char(_value) =>
        <div className="cell">
          { ReasonReact.stringToElement(String.make(1, _value)) }
          { switch (vClue) {
            | None => ReasonReact.nullElement
            | Some(c) => {
              let (ind, _) = c;
              <div className="number">{ ReasonReact.stringToElement(string_of_int(ind)) }</div>;
            }
          } }
        </div>
    }
  } </div>
};

