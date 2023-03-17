import { Flex, Paper, Stack, Text } from "@mantine/core";
import { observer } from "mobx-react";
import { FC, ReactNode } from "react";

interface Props {
  title: string;
  children?: ReactNode;
}

export const Module: FC<Props> = observer(({ title, children }) => {
  return (
    <Paper shadow="lg" p="md" radius="md" style={{ display: "flex" }}>
      <Stack align="stretch" spacing="sm" style={{ flex: 1 }}>
        <Flex style={{ flex: 1 }} align="center">
          {children}
        </Flex>
        <Text weight={500} ta="center">
          {title}
        </Text>
      </Stack>
    </Paper>
  );
});
