#/usr/bin/env python3

# pretty printing stuff
from html import escape
from IPython.display import display, Markdown, HTML, IFrame

def dd(summary, body, hidden=True):
    return f"<details{'' if hidden else ' open'}>\n\n<summary><b>{summary}</b></summary>\n\n{body}\n\n</details>"

def ul(items):
    items_str = "\n</li><li>\n".join([str(v) for v in items])
    return f"<ul><li>{items_str}</li></ul>"

def ol(items):
    items_str = "\n</li><li>\n".join([str(v) for v in items])
    return f"<ol><li>{items_str}</li></ol>"

def bq(body):
    return f"\n\n<blockquote>\n\n{body}\n\n</blockquote>"

def dd_render(i, q, msgs=[], ans=""):
    """Dropdown rendered. Useful mostly to help render answer traces"""
    transcript_lines = []
    for role, content in msgs["messages"]:
        role_lbl = "User" if role == "user" else "AI"
        transcript_lines.append(f"**{role_lbl}:**\n\n{content}\n")
    transcript_md = "\n---\n".join(transcript_lines)
    return dd(
        summary = f'{i}. {escape(q)}',
        body = f'**Final Answer**' + bq(str(ans + "\n\n" + dd('Show transcript (messages)', bq(transcript_md))))
    )

def html_preview(value, unhidden=[], preview_len=200, k=""):
    """A very specifically-implemented preview function. Grossly verfit for this notebook"""
    get_title = lambda v: " ".join(str(v).split()[:3]) + "..." if not isinstance(v, dict) else v.get("title", list(v.values())[0])
    if len(str(value)) < preview_len:
        return value
    if isinstance(value, dict):
        return ul([f"<b>{k}</b>: {v}" if len(str(v)) < preview_len else dd(k, html_preview(v, unhidden, preview_len, k), k not in unhidden) for k, v in value.items()])
    if isinstance(value, list):
        return ol([v if len(str(v)) < preview_len else dd(get_title(v), html_preview(v, unhidden, preview_len), k not in unhidden) for i, v in enumerate(value)])
    return value

display(HTML(dd("This is a dropdown list", "which hides more stuff like" + ul(range(5))) + "Pretty cool, right?"))
