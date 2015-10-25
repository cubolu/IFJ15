## Error Handling

All error handling in IFJ15 is done through the `error` and `warning` functions.

All possible exit return values are defined in `error.h` as per the project requirements. Use these in case of an error in your section.

### API

- `void error(const char *error_str, error_t error_type)`
  - Writes `error_str` to `stderr`, deallocates all remaining dynamically allocated memory in the project, and exits with `error_type` return code.
- `void warning(const char *warn_str)`
  - Writes `warn_str` to `stderr`. Useful for debugging. Shouldn't happen in production ready code unless a non critical error happened.
