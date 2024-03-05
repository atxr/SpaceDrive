# SpaceDrive

## Deploy chall

```bash
sudo docker build -t chall .
sudo docker run --rm -p 5000:5000 -it chall
```

## :triangular_flag_on_post: Challenge Informations



| **Title**                | Notebook                                                                                                                                                                                                                                                                                                                              |
| ------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Category**             | pwn                                                                                                                                                                                                                                                                                                                                   |
| **Description**          | To upload the firmware updates to our satellites, we deplopped SpaceDrive, a safe file uploader across the stars. Because we experienced many issues with malicious ZipFiles in the past, our security team also developped a zip scanner to detected embeded malware. Will you be able to break it to take control of the satellite? |
| **Author**               | atxr                                                                                                                                                                                                                                                                                                                                  |
| **Difficulty (/10)**     | 8                                                                                                                                                                                                                                                                                                                                     |
| **Is Remote**            | Yes                                                                                                                                                                                                                                                                                                                                   |
| **Has attachments**      | Yes                                                                                                                                                                                                                                                                                                                                   |
| **Estimated solve time** | ~2h                                                                                                                                                                                                                                                                                                                                   |
| **Solve instructions**   | First modify the victim/attacker ip in `solve/solve.py`. Then `cd solve && python3 solve.py` and follow the instructions                                                                                                                                                                                                              |
| **Flag**                 | **`THCon{WH3N_y0U_4N7iVIrus_IS_4C7U4LLY_4_84cKd00R}`**                                                                                                                                                                                                                                                                                |
| **Deploy**               | `sudo docker build -t spacedrive . && sudo docker run --rm -p 5000:5000 -it spacedrive`                                                                                                                                                                                                                                               |

