echo ">> Performing auto push to remote github repository"
sleep 2

git add .
git commit -m "automatic message from desktop"
git push origin main