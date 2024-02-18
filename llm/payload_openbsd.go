package llm

import (
	"embed"
)

//go:embed llama.cpp/build/openbsd*/*/*/lib/*.so*
var libEmbed embed.FS
