let readFile = (file) => {
  let ic = open_in(file);
  let n = in_channel_length(ic);
  let fileRaw = really_input_string(ic, n);
  close_in(ic);
  fileRaw
};
